import struct
import serial


SERIAL_PORT = "COM5"
BAUD_RATE = 115200

SYNC_BYTES = b"\xAA\x55"

PROTOCOL_VERSION = 0x01
PACKET_TYPE_ADC_SAMPLE = 0x01

PAYLOAD_LENGTH = 11

PACKET_DATA_SIZE = 18
PACKET_SIZE = 20

ADC_REFERENCE_VOLTAGE = 3.3
ADC_MAX_VALUE = 4095


def crc16_ccitt(data):
    """
    CRC-16/CCITT-FALSE

    Polynomial: 0x1021
    Initial value: 0xFFFF
    """

    crc = 0xFFFF

    for byte in data:

        crc ^= byte << 8

        for _ in range(8):

            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF

    return crc


def read_exact(serial_port, length):

    data = bytearray()

    while len(data) < length:

        chunk = serial_port.read(
            length - len(data)
        )

        if chunk:
            data.extend(chunk)

    return bytes(data)


def find_packet_start(serial_port):

    previous_byte = None

    while True:

        current_byte = serial_port.read(1)

        if not current_byte:
            continue

        current_value = current_byte[0]

        if (
            previous_byte == 0xAA
            and current_value == 0x55
        ):
            return

        previous_byte = current_value


def validate_crc(packet):

    received_crc = struct.unpack_from(
        "<H",
        packet,
        18
    )[0]

    calculated_crc = crc16_ccitt(
        packet[:PACKET_DATA_SIZE]
    )

    return (
        received_crc == calculated_crc,
        received_crc,
        calculated_crc
    )


def decode_adc_packet(packet):

    protocol_version = packet[2]
    packet_type = packet[3]
    payload_length = packet[4]

    if protocol_version != PROTOCOL_VERSION:
        return None

    if packet_type != PACKET_TYPE_ADC_SAMPLE:
        return None

    if payload_length != PAYLOAD_LENGTH:
        return None

    packet_sequence = struct.unpack_from(
        "<H",
        packet,
        5
    )[0]

    sample_number = struct.unpack_from(
        "<I",
        packet,
        7
    )[0]

    adc_value = struct.unpack_from(
        "<H",
        packet,
        11
    )[0]

    dropped_samples = struct.unpack_from(
        "<I",
        packet,
        13
    )[0]

    status_flags = packet[17]

    voltage = (
        adc_value
        * ADC_REFERENCE_VOLTAGE
        / ADC_MAX_VALUE
    )

    return {
        "sequence": packet_sequence,
        "sample": sample_number,
        "adc": adc_value,
        "voltage": voltage,
        "dropped": dropped_samples,
        "status": status_flags,
    }


def main():

    print("Pico 2 W Telemetry Ground Station")
    print("--------------------------------")
    print(f"Opening {SERIAL_PORT}...")

    valid_packets = 0
    crc_errors = 0

    with serial.Serial(
        SERIAL_PORT,
        BAUD_RATE,
        timeout=1
    ) as ser:

        ser.reset_input_buffer()

        print("Connected.")
        print("Waiting for telemetry...\n")

        while True:

            find_packet_start(ser)

            remaining = read_exact(
                ser,
                PACKET_SIZE - 2
            )

            packet = (
                SYNC_BYTES
                + remaining
            )

            crc_ok, received_crc, calculated_crc = (
                validate_crc(packet)
            )

            if not crc_ok:

                crc_errors += 1

                print(
                    "CRC ERROR | "
                    f"Received: 0x{received_crc:04X} | "
                    f"Calculated: 0x{calculated_crc:04X} | "
                    f"Total CRC Errors: {crc_errors}"
                )

                continue

            decoded = decode_adc_packet(packet)

            if decoded is None:
                continue

            valid_packets += 1

            status_text = (
                "BUFFER OVERRUN"
                if decoded["status"] & 0x01
                else "OK"
            )

            print(
                f"SEQ: {decoded['sequence']:5d} | "
                f"SAMPLE: {decoded['sample']:8d} | "
                f"ADC: {decoded['adc']:4d} | "
                f"VOLTAGE: {decoded['voltage']:.3f} V | "
                f"DROPPED: {decoded['dropped']:4d} | "
                f"STATUS: {status_text} | "
                f"CRC: OK"
            )


if __name__ == "__main__":
    main()
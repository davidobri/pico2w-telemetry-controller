import struct
import serial


SERIAL_PORT = "COM5"
BAUD_RATE = 115200

SYNC_BYTES = b"\xAA\x55"

PROTOCOL_VERSION = 0x01
PACKET_TYPE_ADC_SAMPLE = 0x01

PAYLOAD_LENGTH = 11
PACKET_SIZE = 18

ADC_REFERENCE_VOLTAGE = 3.3
ADC_MAX_VALUE = 4095


def read_exact(serial_port, length):
    """
    Read exactly 'length' bytes from the serial port.
    """

    data = bytearray()

    while len(data) < length:
        chunk = serial_port.read(length - len(data))

        if chunk:
            data.extend(chunk)

    return bytes(data)


def find_packet_start(serial_port):
    """
    Search the byte stream for the AA 55 sync sequence.
    """

    previous_byte = None

    while True:
        current_byte = serial_port.read(1)

        if not current_byte:
            continue

        current_value = current_byte[0]

        if previous_byte == 0xAA and current_value == 0x55:
            return

        previous_byte = current_value


def decode_adc_packet(packet):
    """
    Decode one 18-byte ADC telemetry packet.
    """

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

    with serial.Serial(
        SERIAL_PORT,
        BAUD_RATE,
        timeout=1
    ) as ser:

        # Clear anything already buffered by Windows.
        ser.reset_input_buffer()

        print("Connected.")
        print("Waiting for telemetry...\n")

        while True:

            # Locate AA 55 packet synchronization bytes.
            find_packet_start(ser)

            # We already consumed the two sync bytes.
            remaining = read_exact(
                ser,
                PACKET_SIZE - 2
            )

            packet = SYNC_BYTES + remaining

            decoded = decode_adc_packet(packet)

            if decoded is None:
                continue

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
                f"STATUS: {status_text}"
            )


if __name__ == "__main__":
    main()
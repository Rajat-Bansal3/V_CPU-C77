def create_program():
    program = bytearray()
    # LOAD R0, 10
    program.extend([0x01, 0x00, 10])
    # LOAD R1, 20
    program.extend([0x01, 0x01, 20])
    # Add R0 and R1, store in R2
    # LOAD R2, 0
    program.extend([0x01, 0x02, 0])
    # ADD R2, R0 (R2 += R0)
    program.extend([0x02, 0x02, 0x00])
    # ADD R2, R1 (R2 += R1)
    program.extend([0x02, 0x02, 0x01])
    # Compare R2 with 30
    # LOAD R3, 30
    program.extend([0x01, 0x03, 3])
    # CMP R2, R3
    program.extend([0x05, 0x02, 0x03])
    # Jump if equal (to HALT)
    # JZ 22 (address of HALT instruction)
    program.extend([0x06, 25])
    # PRINT R2
    program.extend([0x07, 0x02])
    # HALT
    program.append(0xFF)
    return program
with open("test.rjpc", "wb") as f:
    prog = create_program()
    f.write(prog)

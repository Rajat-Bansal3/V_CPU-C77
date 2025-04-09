import struct

LOAD = 0x01
ADD = 0x02
CMP = 0x05
JZ = 0x06
PRINT = 0x07
HALT = 0xFF

def create_program():
    program = bytearray()
    
    # LOAD R0, 10 (opcode=0x01, reg=0x00, value=10)
    program.append(LOAD)
    program.append(0x00)
    program.extend(struct.pack('<I', 10))  # 4-byte value
    
    # LOAD R1, 20
    program.append(LOAD)
    program.append(0x01)
    program.extend(struct.pack('<I', 20))
    
    # LOAD R2, 0
    program.append(LOAD)
    program.append(0x02)
    program.extend(struct.pack('<I', 0))
    
    # ADD R2, R0 (opcode=0x02, dest=0x02, src=0x00)
    program.extend([ADD, 0x02, 0x00])
    
    # ADD R2, R1
    program.extend([ADD, 0x02, 0x01])
    
    # LOAD R3, 30
    program.append(LOAD)
    program.append(0x03)
    program.extend(struct.pack('<I', 30))
    
    # CMP R2, R3 (opcode=0x05, reg1=0x02, reg2=0x03)
    program.extend([CMP, 0x02, 0x03])
    
    # Calculate address of HALT instruction
    program.extend([PRINT,0x02])
    # Current program length before JZ: len(program)
    
    # JZ (5 bytes) + PRINT (2 bytes) = len(program) + 7
    
    # HALT (opcode=0xFF)
    
    program.append(HALT)
    
    return program

# Write the program to disk
with open("test.rjpc", "wb") as f:
    f.write(create_program())

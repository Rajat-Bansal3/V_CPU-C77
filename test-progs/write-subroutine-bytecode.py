def create_program():
    program = bytearray()
    
    # --- Main Program ---
    # Initialize Stack Pointer (R7) to 0x0000FFFF (top of stack)
    program.extend([0x01, 0x07, 0xFF, 0xFF, 0x00, 0x00])  # LOAD R7, 0x0000FFFF
    
    # Load test values
    program.extend([0x01, 0x00, 0x0A, 0x00, 0x00, 0x00])  # LOAD R0 = 10
    program.extend([0x01, 0x01, 0x14, 0x00, 0x00, 0x00])  # LOAD R1 = 20
    
    # Call multiply subroutine (address 0x0024)
    program.extend([0x0A, 0x00, 0x24])                    # CALL 0x0024
    
    # Print result and halt
    program.extend([0x07, 0x02])                           # PRINT R2
    program.append(0xFF)                                   # HALT
    
    # --- Subroutine 1: Double Number (address 0x0018) ---
    # Input: R0 = number to double
    # Output: R2 = result
    # Pushes R3 to preserve it
    
    # DOUBLE subroutine
    program.extend([0x08, 0x03])                           # PUSH R3 (0x0018)
    program.extend([0x02, 0x00, 0x00])                     # ADD R0, R0 (double)
    program.extend([0x09, 0x03])                           # POP R3
    program.extend([0x0B])                                 # RET
    
    # --- Subroutine 2: Multiply (address 0x0024) ---
    # Input: R0 = a, R1 = b
    # Output: R2 = a * b
    # Uses nested calls to DOUBLE
    
    # MULTIPLY subroutine
    program.extend([0x08, 0x03])                           # PUSH R3 (0x0024)
    program.extend([0x01, 0x02, 0x00, 0x00, 0x00, 0x00])   # LOAD R2 = 0
    
    # Multiplication loop using addition
    program.extend([0x0A, 0x00, 0x18])                     # CALL DOUBLE (0x0018)
    program.extend([0x02, 0x02, 0x01])                     # ADD R2, R1
    program.extend([0x03, 0x00, 0x01])                     # SUB R0, R1 (decrement)
    program.extend([0x05, 0x00, 0x01])                     # CMP R0, R1
    program.extend([0x06, 0x24])                           # JZ 0x24 (loop if not zero)
    
    program.extend([0x09, 0x03])                           # POP R3
    program.extend([0x0B])                                 # RET
    
    return program

with open("test_call.rjpc", "wb") as f:
    prog = create_program()
    f.write(prog)
    print(f"Generated program of {len(prog)} bytes")

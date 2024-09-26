global _start
_start:
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    push QWORD [rsp+8]
    mov rax, 2
    push rax
    pop rbx
    pop rax
    sub rax, rbx
    push rax
    pop rbx
    pop rax
    mul rbx
    push rax
    mov rax, 4
    push rax
    mov rax, 2
    push rax
    pop rbx
    pop rax
    div rbx
    push rax
    pop rbx
    pop rax
    add rax, rbx
    push rax
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label1
    mov rax, 123
    push rax
    push QWORD [rsp+0]
    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
label1:
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label2
    mov rax, 124
    push rax
    push QWORD [rsp+0]
    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
label2:
    mov rax, 1
    push rax
    add rsp, 8
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label3
    mov rax, 0
    push rax
    mov rax, 60
    pop rdi
    syscall
label3:
    mov rax, 1
    push rax
    pop rax
    test rax, rax
    jz label4
    push QWORD [rsp+8]
    push QWORD [rsp+8]
    pop rbx
    pop rax
    add rax, rbx
    push rax
label4:
    mov rax, 1
    push rax
    pop rax
    test rax, rax
    jz label5
    mov rax, 1
    push rax
    pop rax
    test rax, rax
    jz label6
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label7
    mov rax, 3
    push rax
    mov rax, 60
    pop rdi
    syscall
label7:
label6:
label5:
    mov rax, 60
    mov rdi, 0
    syscall
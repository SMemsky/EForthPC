# 65EL02 Boot



## Cold boot

```
POR = 0x2000
BRK = 0x2000

SP = 0x200
PC = 0x400
R = 0x300

E = true
M = true
X = true

mem[0] = 0x02 // Disk RB ID
mem[1] = 0x01 // Console RB ID

// Load boot image
mem[0x400:0x500] = bootImage
```



## Warm boot

```
SP = 0x200
R = 0x300
PC = POR
```



## Boot image description

```
0000: 18fb a500 ef00 c230 a900 03ef 01ef 0264
0010: 02a9 0005 8504 a502 8d80 03e2 20a9 048d
0020: 8203 cbcd 8203 f0fa ad82 03f0 09ef 82e2
0030: 3038 fb4c 0005 c220 a200 035c a040 0042
0040: 9204 e604 e604 88d0 f6a5 04f0 e0e6 024c
0050: 1604
```

```
; Extended mode off
0000: CLC ; clear C flag
0001: XCE ; swap E and C flags

; Setup RedBus with floppy drive at mem[0]
0002: LDA $00
0004: MMU #00

; Reset M and X flags (go 16-bit mode)
0006: REP #30   ; 0b00110000 NOMXDIZC

; Setup RB window at 0x300
0008: LDA #0300 ; 2 bytes, M is not set
000b: MMU #01   ; RB Window = reg A
000d: MMU #02   ; Enable RB
000f: STZ $02   ; [2] = 0, [3] = 0, current disk sector
0011: LDA #0500
0014: STA $04   ; [4] = 0, [5] = 5, current write offset
0016: LDA $02
0018: STA $0380
001b: SEP #20   ; 0b00100000, set flag M (8-bit mode)
; CPU now in 8-bit mode
001d: LDA #04
001f: STA $0382 ; Read disk sector

waitSetSector:
0022: WAI ; Wait next cycle
0023: CMP $0382 ; Compare result to reg A
0026: BEQ waitSetSector

0028: LDA $0382
002b: BEQ setSectorOk   ; jump if zero

; Failed to get sector, disable RB
sectorReadFail:
002d: MMU #82
002f: SEP #30   ; Set M, X
0031: SEC
0032: XCE       ; Go to compat mode and try to boot from 0x500
0033: JMP $0500 ; 

setSectorOk:
0036: REP #20   ; Reset M flag
0038: LDX #0300
003b: TXI
003c: LDY #0040
readLoop:
003f: NXA       ; A = mem[I], i += 2
0040: STA ($04) ; mem[mem[4:5]] = A
0042: INC $04
0044: INC $04
0046: DEY
0047: BNE readLoop  ; jump while Y != 0
0049: LDA $04
004b: BEQ sectorReadFail    ; jump if [4] == 0
004d: INC $02
004f: JMP $0416 ; boot+0x16
```

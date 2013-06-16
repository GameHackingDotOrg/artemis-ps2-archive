//============================================================
// Decompress Font Character
/*
Input:
   a0 = Character Ascii (eg. 'a')
   a1 = Decompressed Destination
Output:
   v0 = 0 (success) 1 (failed)
*/
FontChar_Decompress:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)

addiu v0, zero, 12
multu a0, a0, v0

setreg s0, :FontCharacters
daddu s0, s0, a0
daddu s1, a1, zero

addiu s2, zero, 10
_FontChar_Decompress_Loop:

jal :ByteToBIN
lb a0, $0000(s0)

sw v0, $0000(s1)

addiu s0, s0, 1
addiu s1, s1, 4
bgtz s2, :_FontChar_Decompress_Loop
addiu s2, s2, -1

daddu v0, zero, zero
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
jr ra
addiu sp, sp, $0080


//============================================================
FontCharacters:

FontChar_00:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_01:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_02:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_03:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_04:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_05:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_06:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_07:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_08:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_09:
hexcode $00000000
hexcode $00000000
hexcode $00000000

FontChar_0A:
hexcode $00000000
hexcode $00000000
hexcode $00000000

FontChar_0B:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_0C:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_0D:
hexcode $00000000
hexcode $00000000
hexcode $00000000

FontChar_0E:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_0F:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_10:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_11:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_12:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_13:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_14:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_15:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_16:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_17:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_18:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_19:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1A:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1B:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1C:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1D:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1E:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_1F:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_20:
hexcode $00000000
hexcode $00000000
hexcode $00000000

FontChar_21:
hexcode $08080000
hexcode $00080808
hexcode $00000008

FontChar_22:
hexcode $12121200
hexcode $00000000
hexcode $00000000

FontChar_23:
hexcode $28280000
hexcode $0A3F143E
hexcode $0000000A

FontChar_24:
hexcode $0A3C0800
hexcode $2828180E
hexcode $0000081E

FontChar_25:
hexcode $25420000
hexcode $52542A15
hexcode $00000021

FontChar_26:
hexcode $0A040000
hexcode $3149460A
hexcode $0000003E

FontChar_27:
hexcode $08080800
hexcode $00000000
hexcode $00000000

FontChar_28:
hexcode $04083000
hexcode $04040404
hexcode $00300804

FontChar_29:
hexcode $10080600
hexcode $10101010
hexcode $00060810

FontChar_2A:
hexcode $36080000
hexcode $0000140C
hexcode $00000000

FontChar_2B:
hexcode $08000000
hexcode $08083E08
hexcode $00000008

FontChar_2C:
hexcode $00000000
hexcode $0C000000
hexcode $0004080C

FontChar_2D:
hexcode $00000000
hexcode $00003E00
hexcode $00000000

FontChar_2E:
hexcode $00000000
hexcode $0C000000
hexcode $0000000C

FontChar_2F:
hexcode $30204000
hexcode $04080810
hexcode $00010206

FontChar_30:
hexcode $221C0000
hexcode $22222222
hexcode $0000001C

FontChar_31:
hexcode $0A0C0000
hexcode $08080808
hexcode $0000003E

FontChar_32:
hexcode $100E0000
hexcode $02040810
hexcode $0000001E

FontChar_33:
hexcode $101E0000
hexcode $10100C10
hexcode $0000000E

FontChar_34:
hexcode $18100000
hexcode $103E1214
hexcode $00000010

FontChar_35:
hexcode $021E0000
hexcode $10100E02
hexcode $0000000E

FontChar_36:
hexcode $04380000
hexcode $22261A02
hexcode $0000001C

FontChar_37:
hexcode $103E0000
hexcode $04040810
hexcode $00000002

FontChar_38:
hexcode $221C0000
hexcode $22221C12
hexcode $0000001C

FontChar_39:
hexcode $221C0000
hexcode $10203C22
hexcode $0000000E

FontChar_3A:
hexcode $0C000000
hexcode $0C00000C
hexcode $0000000C

FontChar_3B:
hexcode $0C000000
hexcode $0C00000C
hexcode $0004080C

FontChar_3C:
hexcode $20000000
hexcode $18060618
hexcode $00000020

FontChar_3D:
hexcode $00000000
hexcode $007F007F
hexcode $00000000

FontChar_3E:
hexcode $01000000
hexcode $06181806
hexcode $00000001

FontChar_3F:
hexcode $221E0000
hexcode $00081020
hexcode $00000008

FontChar_40:
hexcode $223C0000
hexcode $027D2539
hexcode $0000001C

FontChar_41:
hexcode $14080000
hexcode $223E2214
hexcode $00000041

FontChar_42:
hexcode $221E0000
hexcode $22221E22
hexcode $0000001E

FontChar_43:
hexcode $023C0000
hexcode $02010101
hexcode $0000003C

FontChar_44:
hexcode $221E0000
hexcode $22222222
hexcode $0000001E

FontChar_45:
hexcode $023E0000
hexcode $02021E02
hexcode $0000003E

FontChar_46:
hexcode $023E0000
hexcode $02021E02
hexcode $00000002

FontChar_47:
hexcode $023C0000
hexcode $22213101
hexcode $0000003C

FontChar_48:
hexcode $22220000
hexcode $22223E22
hexcode $00000022

FontChar_49:
hexcode $083E0000
hexcode $08080808
hexcode $0000003E

FontChar_4A:
hexcode $101C0000
hexcode $10101010
hexcode $0000000E

FontChar_4B:
hexcode $12220000
hexcode $120A060A
hexcode $00000022

FontChar_4C:
hexcode $02020000
hexcode $02020202
hexcode $0000003E

FontChar_4D:
hexcode $33330000
hexcode $252D2D2B
hexcode $00000021

FontChar_4E:
hexcode $26220000
hexcode $32322A2E
hexcode $00000022

FontChar_4F:
hexcode $211E0000
hexcode $21212121
hexcode $0000001E

FontChar_50:
hexcode $221E0000
hexcode $02021E22
hexcode $00000002

FontChar_51:
hexcode $211E0000
hexcode $21212121
hexcode $0060301E

FontChar_52:
hexcode $110F0000
hexcode $11090F11
hexcode $00000021

FontChar_53:
hexcode $023C0000
hexcode $20201C02
hexcode $0000001E

FontChar_54:
hexcode $087F0000
hexcode $08080808
hexcode $00000008

FontChar_55:
hexcode $22220000
hexcode $22222222
hexcode $0000001C

FontChar_56:
hexcode $22410000
hexcode $14142222
hexcode $00000008

FontChar_57:
hexcode $49410000
hexcode $32365649
hexcode $00000022

FontChar_58:
hexcode $22410000
hexcode $22140814
hexcode $00000041

FontChar_59:
hexcode $22410000
hexcode $08080814
hexcode $00000008

FontChar_5A:
hexcode $203F0000
hexcode $02040810
hexcode $0000003F

FontChar_5B:
hexcode $04041C00
hexcode $04040404
hexcode $001C0404

FontChar_5C:
hexcode $02020100
hexcode $10080804
hexcode $00402010

FontChar_5D:
hexcode $08080E00
hexcode $08080808
hexcode $000E0808

FontChar_5E:
hexcode $14080800
hexcode $00412236
hexcode $00000000

FontChar_5F:
hexcode $00000000
hexcode $00000000
hexcode $00007F00

FontChar_60:
hexcode $00000804
hexcode $00000000
hexcode $00000000

FontChar_61:
hexcode $1C000000
hexcode $22223C20
hexcode $0000007C

FontChar_62:
hexcode $1A020200
hexcode $22222226
hexcode $0000001E

FontChar_63:
hexcode $3C000000
hexcode $02020202
hexcode $0000003C

FontChar_64:
hexcode $3C202000
hexcode $22222222
hexcode $0000003C

FontChar_65:
hexcode $1C000000
hexcode $02023E22
hexcode $0000003C

FontChar_66:
hexcode $7E087000
hexcode $08080808
hexcode $00000008

FontChar_67:
hexcode $3C000000
hexcode $22222222
hexcode $001C203C

FontChar_68:
hexcode $3A020200
hexcode $22222226
hexcode $00000022

FontChar_69:
hexcode $0E000800
hexcode $08080808
hexcode $00000008

FontChar_6A:
hexcode $1C001000
hexcode $10101010
hexcode $000E1010

FontChar_6B:
hexcode $22020200
hexcode $120A0E12
hexcode $00000022

FontChar_6C:
hexcode $08080E00
hexcode $08080808
hexcode $00000008

FontChar_6D:
hexcode $6D000000
hexcode $4949495B
hexcode $00000049

FontChar_6E:
hexcode $3A000000
hexcode $22222226
hexcode $00000022

FontChar_6F:
hexcode $1C000000
hexcode $22222222
hexcode $0000001C

FontChar_70:
hexcode $1A000000
hexcode $22222226
hexcode $0002021E

FontChar_71:
hexcode $3C000000
hexcode $22222222
hexcode $0020203C

FontChar_72:
hexcode $3A000000
hexcode $02020226
hexcode $00000002

FontChar_73:
hexcode $1E000000
hexcode $10180602
hexcode $0000000E

FontChar_74:
hexcode $3E040000
hexcode $04040404
hexcode $00000038

FontChar_75:
hexcode $22000000
hexcode $32222222
hexcode $0000002E

FontChar_76:
hexcode $41000000
hexcode $14142222
hexcode $00000008

FontChar_77:
hexcode $41000000
hexcode $36555549
hexcode $00000022

FontChar_78:
hexcode $21000000
hexcode $120C0C12
hexcode $00000021

FontChar_79:
hexcode $41000000
hexcode $14142222
hexcode $00070C08

FontChar_7A:
hexcode $3E000000
hexcode $04081020
hexcode $0000003E

FontChar_7B:
hexcode $08083000
hexcode $08080C08
hexcode $00300808

FontChar_7C:
hexcode $08080800
hexcode $08080808
hexcode $00080808

FontChar_7D:
hexcode $08080600
hexcode $08081808
hexcode $000E0808

FontChar_7E:
hexcode $00000000
hexcode $00394E00
hexcode $00000000

FontChar_7F:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_80:
hexcode $221C0000
hexcode $220F020F
hexcode $0000001C

FontChar_81:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_82:
hexcode $00000000
hexcode $0C000000
hexcode $0004080C

FontChar_83:
hexcode $3C080838
hexcode $08080808
hexcode $000E0808

FontChar_84:
hexcode $00000000
hexcode $00000000
hexcode $00121212

FontChar_85:
hexcode $00000000
hexcode $00000000
hexcode $0000002A

FontChar_86:
hexcode $08080000
hexcode $0808083E
hexcode $00080808

FontChar_87:
hexcode $08080000
hexcode $3E08083E
hexcode $00080808

FontChar_88:
hexcode $0000120C
hexcode $00000000
hexcode $00000000

FontChar_89:
hexcode $0A120000
hexcode $2C2C080A
hexcode $0000002A

FontChar_8A:
hexcode $023C0C12
hexcode $20201C02
hexcode $0000001E

FontChar_8B:
hexcode $00000000
hexcode $08040810
hexcode $00000010

FontChar_8C:
hexcode $097E0000
hexcode $09093909
hexcode $0000007E

FontChar_8D:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_8E:
hexcode $203F0C12
hexcode $02040810
hexcode $0000003F

FontChar_8F:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_90:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_91:
hexcode $0C040800
hexcode $0000000C
hexcode $00000000

FontChar_92:
hexcode $080C0C00
hexcode $00000004
hexcode $00000000

FontChar_93:
hexcode $12121200
hexcode $00000000
hexcode $00000000

FontChar_94:
hexcode $12121200
hexcode $00000000
hexcode $00000000

FontChar_95:
hexcode $00000000
hexcode $001C1C1C
hexcode $00000000

FontChar_96:
hexcode $00000000
hexcode $00003E00
hexcode $00000000

FontChar_97:
hexcode $00000000
hexcode $00007F00
hexcode $00000000

FontChar_98:
hexcode $00001428
hexcode $00000000
hexcode $00000000

FontChar_99:
hexcode $3A2F0000
hexcode $0000002A
hexcode $00000000

FontChar_9A:
hexcode $1E000C12
hexcode $10180602
hexcode $0000000E

FontChar_9B:
hexcode $00000000
hexcode $08100804
hexcode $00000004

FontChar_9C:
hexcode $1E000000
hexcode $09093929
hexcode $0000003E

FontChar_9D:
hexcode $121E0000
hexcode $12121212
hexcode $0000001E

FontChar_9E:
hexcode $3E000C12
hexcode $04081020
hexcode $0000003E

FontChar_9F:
hexcode $22410012
hexcode $08080814
hexcode $00000008

FontChar_A0:
hexcode $00000000
hexcode $00000000
hexcode $00000000

FontChar_A1:
hexcode $08000000
hexcode $08080800
hexcode $00000808

FontChar_A2:
hexcode $1C080000
hexcode $1C0A0A0A
hexcode $00000008

FontChar_A3:
hexcode $04180000
hexcode $04040E04
hexcode $0000001E

FontChar_A4:
hexcode $1E210000
hexcode $211E1212
hexcode $00000000

FontChar_A5:
hexcode $22410000
hexcode $3E083E14
hexcode $00000008

FontChar_A6:
hexcode $08080800
hexcode $00000000
hexcode $00080808

FontChar_A7:
hexcode $021C0000
hexcode $14121A06
hexcode $000E1018

FontChar_A8:
hexcode $00000012
hexcode $00000000
hexcode $00000000

FontChar_A9:
hexcode $211E0000
hexcode $212D252D
hexcode $0000001E

FontChar_AA:
hexcode $1C1E0000
hexcode $00003E12
hexcode $00000000

FontChar_AB:
hexcode $00000000
hexcode $140A1428
hexcode $00000028

FontChar_AC:
hexcode $00000000
hexcode $20203F00
hexcode $00000000

FontChar_AD:
hexcode $00000000
hexcode $00003E00
hexcode $00000000

FontChar_AE:
hexcode $110E0000
hexcode $000E1117
hexcode $00000000

FontChar_AF:
hexcode $0000007F
hexcode $00000000
hexcode $00000000

FontChar_B0:
hexcode $14080000
hexcode $00000008
hexcode $00000000

FontChar_B1:
hexcode $08000000
hexcode $08083E08
hexcode $0000003E

FontChar_B2:
hexcode $183C0000
hexcode $0000003C
hexcode $00000000

FontChar_B3:
hexcode $081C0000
hexcode $0000001C
hexcode $00000000

FontChar_B4:
hexcode $00000810
hexcode $00000000
hexcode $00000000

FontChar_B5:
hexcode $22000000
hexcode $32222222
hexcode $0002022E

FontChar_B6:
hexcode $2E3E0000
hexcode $28282C2E
hexcode $00282828

FontChar_B7:
hexcode $00000000
hexcode $000C0C00
hexcode $00000000

FontChar_B8:
hexcode $00000000
hexcode $00000000
hexcode $00181000

FontChar_B9:
hexcode $080C0000
hexcode $00000008
hexcode $00000000

FontChar_BA:
hexcode $120C0000
hexcode $00000C12
hexcode $00000000

FontChar_BB:
hexcode $00000000
hexcode $1428140A
hexcode $0000000A

FontChar_BC:
hexcode $0A130000
hexcode $7A2A340A
hexcode $00000021

FontChar_BD:
hexcode $0A130000
hexcode $2242740A
hexcode $00000071

FontChar_BE:
hexcode $23470000
hexcode $7A2C3817
hexcode $00000021

FontChar_BF:
hexcode $08000000
hexcode $04080800
hexcode $003C2202

FontChar_C0:
hexcode $14080402
hexcode $223E2214
hexcode $00000041

FontChar_C1:
hexcode $14080810
hexcode $223E2214
hexcode $00000041

FontChar_C2:
hexcode $1408120C
hexcode $223E2214
hexcode $00000041

FontChar_C3:
hexcode $14081428
hexcode $223E2214
hexcode $00000041

FontChar_C4:
hexcode $14080012
hexcode $223E2214
hexcode $00000041

FontChar_C5:
hexcode $14081408
hexcode $223E2214
hexcode $00000041

FontChar_C6:
hexcode $0C380000
hexcode $0E0A1A0C
hexcode $00000039

FontChar_C7:
hexcode $023C0000
hexcode $02010101
hexcode $0018103C

FontChar_C8:
hexcode $023E0804
hexcode $02021E02
hexcode $0000003E

FontChar_C9:
hexcode $023E1020
hexcode $02021E02
hexcode $0000003E

FontChar_CA:
hexcode $023E120C
hexcode $02021E02
hexcode $0000003E

FontChar_CB:
hexcode $023E0012
hexcode $02021E02
hexcode $0000003E

FontChar_CC:
hexcode $083E0804
hexcode $08080808
hexcode $0000003E

FontChar_CD:
hexcode $083E0810
hexcode $08080808
hexcode $0000003E

FontChar_CE:
hexcode $083E120C
hexcode $08080808
hexcode $0000003E

FontChar_CF:
hexcode $083E0012
hexcode $08080808
hexcode $0000003E

FontChar_D0:
hexcode $221E0000
hexcode $22222722
hexcode $0000001E

FontChar_D1:
hexcode $26221428
hexcode $32322A2E
hexcode $00000022

FontChar_D2:
hexcode $211E0804
hexcode $21212121
hexcode $0000001E

FontChar_D3:
hexcode $211E0810
hexcode $21212121
hexcode $0000001E

FontChar_D4:
hexcode $211E120C
hexcode $21212121
hexcode $0000001E

FontChar_D5:
hexcode $211E1428
hexcode $21212121
hexcode $0000001E

FontChar_D6:
hexcode $211E0012
hexcode $21212121
hexcode $0000001E

FontChar_D7:
hexcode $21000000
hexcode $120C0C12
hexcode $00000021

FontChar_D8:
hexcode $333E0000
hexcode $33252529
hexcode $0000001F

FontChar_D9:
hexcode $22220804
hexcode $22222222
hexcode $0000001C

FontChar_DA:
hexcode $22221020
hexcode $22222222
hexcode $0000001C

FontChar_DB:
hexcode $2222120C
hexcode $22222222
hexcode $0000001C

FontChar_DC:
hexcode $22220012
hexcode $22222222
hexcode $0000001C

FontChar_DD:
hexcode $22411020
hexcode $08080814
hexcode $00000008

FontChar_DE:
hexcode $1E020000
hexcode $1E222222
hexcode $00000002

FontChar_DF:
hexcode $12121C00
hexcode $22120A0A
hexcode $0000003A

FontChar_E0:
hexcode $1C000804
hexcode $22223C20
hexcode $0000007C

FontChar_E1:
hexcode $1C000810
hexcode $22223C20
hexcode $0000007C

FontChar_E2:
hexcode $1C00120C
hexcode $22223C20
hexcode $0000007C

FontChar_E3:
hexcode $1C001428
hexcode $22223C20
hexcode $0000007C

FontChar_E4:
hexcode $1C001400
hexcode $22223C20
hexcode $0000007C

FontChar_E5:
hexcode $1C081408
hexcode $22223C20
hexcode $0000007C

FontChar_E6:
hexcode $37000000
hexcode $09093E28
hexcode $00000036

FontChar_E7:
hexcode $3C000000
hexcode $02020202
hexcode $0018103C

FontChar_E8:
hexcode $1C000804
hexcode $02023E22
hexcode $0000003C

FontChar_E9:
hexcode $1C000810
hexcode $02023E22
hexcode $0000003C

FontChar_EA:
hexcode $1C00120C
hexcode $02023E22
hexcode $0000003C

FontChar_EB:
hexcode $1C001400
hexcode $02023E22
hexcode $0000003C

FontChar_EC:
hexcode $0E000804
hexcode $08080808
hexcode $00000008

FontChar_ED:
hexcode $0E000810
hexcode $08080808
hexcode $00000008

FontChar_EE:
hexcode $0E00120C
hexcode $08080808
hexcode $00000008

FontChar_EF:
hexcode $0E001400
hexcode $08080808
hexcode $00000008

FontChar_F0:
hexcode $1C120E00
hexcode $22222222
hexcode $0000001C

FontChar_F1:
hexcode $3A001428
hexcode $22222226
hexcode $00000022

FontChar_F2:
hexcode $1C000804
hexcode $22222222
hexcode $0000001C

FontChar_F3:
hexcode $1C000408
hexcode $22222222
hexcode $0000001C

FontChar_F4:
hexcode $1C00120C
hexcode $22222222
hexcode $0000001C

FontChar_F5:
hexcode $1C001428
hexcode $22222222
hexcode $0000001C

FontChar_F6:
hexcode $1C001400
hexcode $22222222
hexcode $0000001C

FontChar_F7:
hexcode $04000000
hexcode $00003F00
hexcode $00000004

FontChar_F8:
hexcode $3C000000
hexcode $26262A32
hexcode $0000001E

FontChar_F9:
hexcode $22000804
hexcode $32222222
hexcode $0000002E

FontChar_FA:
hexcode $22000408
hexcode $32222222
hexcode $0000002E

FontChar_FB:
hexcode $2200120C
hexcode $32222222
hexcode $0000002E

FontChar_FC:
hexcode $22001400
hexcode $32222222
hexcode $0000002E

FontChar_FD:
hexcode $41000810
hexcode $14142222
hexcode $00070C08

FontChar_FE:
hexcode $1A020200
hexcode $22222226
hexcode $0002021E

FontChar_FF:
hexcode $41001400
hexcode $14142222
hexcode $00070C08
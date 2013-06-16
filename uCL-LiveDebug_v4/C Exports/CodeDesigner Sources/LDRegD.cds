/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/

/*
NOTE: This was not designed to be changed.
If it does get changed; be sure to update the Engine with the new lines regarding the replaced line and the j $RETURN (commented below).
You will also have to update the size as well. THese things are in the _installDumper and _PlaceDumper subroutines within LDv3Engine.cds
*/


//THe address isn't really important since there aren't any jumps
address $00000000

GO:
//From Gtlcpimp's Exception handler
addiu sp, sp, $FFD0
sq k0, $0000(sp)
sq at, $0010(sp)
sq k1, $0020(sp)

lui k0, $0000
ori k0, k0, $0000 //This will be replaced by the installer later

sq at, $0000(k0)
sq v0, $0010(k0)
sq v1, $0020(k0)
sq a0, $0030(k0)
sq a1, $0040(k0)
sq a2, $0050(k0)
sq a3, $0060(k0)
sq t0, $0070(k0)
sq t1, $0080(k0)
sq t2, $0090(k0)
sq t3, $00A0(k0)
sq t4, $00B0(k0)
sq t5, $00C0(k0)
sq t6, $00D0(k0)
sq t7, $00E0(k0)
sq s0, $00F0(k0)
sq s1, $0100(k0)
sq s2, $0110(k0)
sq s3, $0120(k0)
sq s4, $0130(k0)
sq s5, $0140(k0)
sq s6, $0150(k0)
sq s7, $0160(k0)
sq t8, $0170(k0)
sq t9, $0180(k0)
sq gp, $01B0(k0)
sq fp, $01C0(k0)
sq ra, $01E0(k0)

addiu at, sp, $0030
sq at, $01D0(k0)
sq k1, $01A0(k0)

lq k1, $0000(sp)
sq k1, $0190(k0)

lq k0, $0000(sp)
lq at, $0010(sp)
lq k1, $0020(sp)

nop //operation replaced with j $000A0000 - When dumper installed
addiu sp, sp, $0030
nop //a j to the installed address + 8


#!/usr/bin/perl
# Add symbols to disassembled file:
use strict;

if ($#ARGV != 0) {
	print "Usage: $0 [disassembler file]\n";
	print "\n";
	print "Add symbols to a disassembled irx file.\n";
	print "The files need to be generated with:\n";
	print "iop-objdump -D [irx module]\n";
	print "irxtool -m [iry module] >[map file]\n";
	exit 1;
}

my $file = $ARGV[0];
my %map;
my $i;

$i = 0;

open(infile, "irxtool -m $file |");
while(<infile>) {
	#print $_ . "\n";
	if ($_ =~ /0x0*([^0 ]*[0-9]*) (.*)/) {
		#print $1 . ":::" . $2 . "\n";
		$map{$1} = $2;
	}
	$i += 1;
}
close(infile);

open(infile, "iop-objdump -D $file |");
while(<infile>) {
	my $newline;

	$newline = $_;
	foreach my $addr (keys(%map)) {
		$_ =~ s/0x$addr/$map{$addr}/g;
		if ($_ =~ /^ *$addr:/) {
			print $map{$addr} . ":\n";
		}
	}
	print $_;
}
close(infile);


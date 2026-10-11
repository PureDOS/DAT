#!/usr/bin/perl

#--------------------------------------------#
# dos-pure-dat verification script           #
# License: Public Domain (www.unlicense.org) #
#--------------------------------------------#

use strict;

our $datdir = (($0 =~ /(.*[\/\\])/)[0])."../..";

our ($errcount, $mname, $ingame) = (0, 0);
sub Err
{
	print STDERR substr($mname, length($datdir) + 1).":".int($_[1]).": error: ".($ingame ? "Game '$ingame': " : "").$_[0]."\n";
	$errcount++;
}

our (@files, %gamedb, $numxml, $numdosc);
opendir(DIR, "$datdir");                 push(@files, map { "$datdir/".$_                 } grep { /\.xml$/i  } readdir(DIR)); closedir(DIR);
opendir(DIR, "$datdir/dosc");            push(@files, map { "$datdir/dosc/".$_            } grep { /\.dosc$/i } readdir(DIR)); closedir(DIR);
opendir(DIR, "$datdir/unverified");      push(@files, map { "$datdir/unverified/".$_      } grep { /\.xml$/i  } readdir(DIR)); closedir(DIR);
opendir(DIR, "$datdir/unverified/dosc"); push(@files, map { "$datdir/unverified/dosc/".$_ } grep { /\.dosc$/i } readdir(DIR)); closedir(DIR);

foreach my $m (@files)
{
	$mname = $m;
	my $mcontents = do { open my $fh, '<:raw', $mname; local $/; <$fh> };
	if ($mname =~ /\.xml$/)
	{
		my ($dat, $i, $cut_r, $unverified) = ($mcontents, 0, (index($mcontents, "\r") == -1 ? 0 : 1), ($mname =~ /PureDOSDAT_Unverified/));
		my $XMLErr = sub { my ($ln, $p) = (1, -1); $ln++ while (($p = index($dat, "\n", $p+1)) >= 0 && $p < $i); Err($_[0], $ln); };
		for (my ($iNext, $iEnd, $lastingame, $inheader, $inrom, $rommeta, $x, $y, $z, $g, @filearr) = (0, length($dat)); $i < $iEnd; $i = ($iNext == -1 ? $iEnd : $iNext + 1))
		{
			$iNext = index($dat, "\n", $i);
			while (index($dat, "\t", $i) == $i) { $i++; }
			my $ln = substr($dat, $i, ($iNext == -1 ? $iEnd : ($iNext - $cut_r)) - $i);
			if ($ingame)
			{
				if ($ln =~ /^<rom name="([^\"]+)" size="(\d+)" crc="([0-9a-f]{8})" md5="([0-9a-f]{32})" sha1="([0-9a-f]{40})"(?: date="(\d{4})-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)"|)(?: data="([^\"]+)"|)(\/?)>$/)
				{
					my ($fname, $fsize, $fcrc, $fmd5, $fsha1, $fyear,$fmon,$fday,$fhour,$fmin,$fsec, $fdata, $romslash) = ($1, int($2), $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
					$fname =~ s/\\/\//g;$fname=~s/\&amp;/\&/g;$fname=~s/\&\#(\d+);/pack("C",$1)/eg;$fname=~s/\&lt;/</g;$fname=~s/\&quot;/\"/g;$fname=~s/\&gt;/>/g;$fname=~s/\&apos;/\'/g;
					if ($fname =~ /([\x7F-\xFF])/) { $XMLErr->("Bad character contained in file list (in '$fname')"); }
					if (index($fname, ".parent") >= 0)
					{
						if ($fsize) { $XMLErr->("Parent file '$fname' has size $fsize (should be 0)"); }
						if ($fmon) { $XMLErr->("Parent file '$fname' has a date (should not be set)"); }
						if (index(join(",", @filearr), ".parent") >= 0) { $XMLErr->("Game has multiple .parent files"); }
					}
					if (index($fname, ".savename") >= 0)
					{
						if ($fsize) { $XMLErr->("Savename file '$fname' has size $fsize (should be 0)"); }
						if ($fmon) { $XMLErr->("Savename file '$fname' has a date (should not be set)"); }
						if (index(join(",", @filearr), ".savename") >= 0) { $XMLErr->("Game has multiple .savename files"); }
					}
					push(@filearr, '"'.$fname.'"');
					$inrom = ($romslash ? undef : $fname);
				}
				elsif ($ln =~ /^<source type="([^\"]+)" frames="([^\"]+)"(?: pregap="([^\"]+)"|)(?: omitted_pregap="([^\"]+)"|) duration="([^\"]+)"(?: size="([^\"]+)" crc="([^\"]+)" md5="([^\"]+)" sha1="([^\"]+)" in_zeros="([^\"]+)" out_zeros="([^\"]+)" quality="([^\"]+)"|)(?: non_silence_pregap="([^\"]+)"|)\/>$/ && $inrom)
				{
					my ($stype, $sframes, $spregap, $somittedpregap, $sduration, $ssize, $scrc, $smd5, $ssha1, $sinzeros, $soutzeros, $squality, $snonsilencepregap) = ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
					if (!$rommeta) { $rommeta = 'S'; } elsif (substr($rommeta, 0, 1) ne 'S') { $XMLErr->("Mixed child-tags in <rom> '$inrom'"); }
					$rommeta .= "#$stype#$sframes#$spregap#$somittedpregap#$sduration#$ssize#$scrc#$smd5#$ssha1#$sinzeros#$soutzeros#$squality#$snonsilencepregap";
				}
				elsif ($ln =~ /^<track number="([^\"]+)" type="([^\"]+)" frames="([^\"]+)"(?: pregap="([^\"]+)"|)(?: omitted_pregap="([^\"]+)"|) duration="([^\"]+)" size="([^\"]+)" crc="([^\"]+)" md5="([^\"]+)" sha1="([^\"]+)"(?: in_zeros="([^\"]+)" out_zeros="([^\"]+)"|)(?: trimmed_crc="([^\"]+)"|)\/>$/ && $inrom)
				{
					my ($tnumber, $ttype, $tframes, $tpregap, $tomittedpregap, $tduration, $tsize, $tcrc, $tmd5, $tsha1, $tinzeros, $toutzeros, $ttrimmedcrc) = ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
					if (!$rommeta) { $rommeta = 'T'; } elsif (substr($rommeta, 0, 1) ne 'T') { $XMLErr->("Mixed child-tags in <rom> '$inrom'"); }
					$rommeta .= "#$tnumber#$ttype#$tframes#$tpregap#$tomittedpregap#$tduration#$tsize#$tcrc#$tmd5#$tsha1#$tinzeros#$toutzeros#$ttrimmedcrc";
				}
				elsif ($ln =~ /^<patch data="([^\"]+)" size="([^\"]+)" crc="([^\"]+)" md5="([^\"]+)" sha1="([^\"]+)"\/>$/ && $inrom)
				{
					my ($pdata, $psize, $pcrc, $pmd5, $psha1) = ($1, $2, $3, $4, $5);
					if (!$rommeta) { $rommeta = 'P'; } elsif (substr($rommeta, 0, 1) ne 'P') { $XMLErr->("Mixed child-tags in <rom> '$inrom'"); }
					$rommeta .= "#$pdata#$psize#$pcrc#$pmd5#$psha1";
				}
				elsif ($ln =~ /^<\/rom>$/ && $inrom)
				{
					if ($rommeta) { $rommeta = ''; }
					$inrom = 0;
				}
				elsif ($ln =~ /^<\/game>$/)
				{
					my ($description, $year, $developer) = ($g->{description}, $g->{year}, $g->{developer});
					if (!$developer) { $XMLErr->("Missing <developer> element"); }
					if (!$year) { $XMLErr->("Missing <year> element"); }
					if (!$description) { $XMLErr->("Missing <description> element"); }
					elsif ($description =~ / - / && $description !~ /:/ && !$unverified) { } # reported with Err below
					elsif ($description =~ /, (The|An|A)/ && !$unverified) { } # reported with Err below
					elsif (!$unverified) # too many errors in unverified
					{
						my $expectdesc = $ingame; $expectdesc =~ s/^(.*?), (The|An|A)/\2 \1/;
						my $expectname = $description.' ('.$year.') ('.$developer.')'.($g->{variant}?' ('.$g->{variant}.')' : ''); $expectname =~ s/\: / - /;
						if ($expectdesc ne $expectname) { $XMLErr->("Game name doesn't match 'DESCRIPTION (YEAR) (DEVELOPER) (VARIANT)' template (expected  '$expectname')"); }
					}

					my $filars = join(",", @filearr);
					if (!$filars) { $XMLErr->("Not a single <rom> element"); }
					my $parent = $g->{parent};
					if    ($parent && index($filars, '"'.$parent.'.parent"') < 0) { $XMLErr->("Entry has a <parent> element but '".$parent.".parent' file is missing"); }
					elsif (!$parent && index($filars, '.parent"') >= 0)           { $XMLErr->("Entry has a .parent file but <parent> element is missing"); }
					if ($parent) { $XMLErr->("Parent '$parent' doesn't end with '.dosz'") unless $parent =~ s/\.dosz$//; push(@{$gamedb{$parent}->{children}}, $ingame); }

					$g->{files} = $filars || "-";
					($ingame, $g, @filearr) = (0);
				}
				elsif ($ln =~ /^<(description|year|comment|comment_dosc|developer|parent|variant)>([^<]+)<\/\1>$/)
				{
					($x, $y) = ($1, $2);
					metafield:
					$XMLErr->("Multiple <$x> fields") if $g->{$x};
					$y=~s/\&amp;/\&/g;$y=~s/\&\#(\d+);/pack("C",$1)/eg;$y=~s/\&lt;/</g;$y=~s/\&quot;/\"/g;$y=~s/\&gt;/>/g;$y=~s/\&apos;/\'/g;
					$g->{$x} = $y;
					if ($x eq 'description')
					{
						if ($y =~ / - / && $y !~ /:/ && !$unverified) { $XMLErr->("In the <description> tag ('$y') the first occurence of ' - ' should be changed to ': '"); }
						if ($y =~ /, (The|An|A)/ && !$unverified) { $XMLErr->("In the <description> tag ('$y') the common article '$1' should be at the beginning not like ', $1'");  }
					}
				}
				elsif ($ln eq "<comment>")      { my $j = index($dat, "</", $iNext); if (substr($dat, $j, 10) ne "</comment>"     ) { $XMLErr->("Bad <comment> tag");      next; } ($x, $y) = ("comment",      substr($dat, $iNext+1, $j-$iNext-1)); $iNext = $j + 10 + $cut_r; goto metafield; }
				elsif ($ln eq "<comment_dosc>") { my $j = index($dat, "</", $iNext); if (substr($dat, $j, 15) ne "</comment_dosc>") { $XMLErr->("Bad <comment_desc> tag"); next; } ($x, $y) = ("comment_dosc", substr($dat, $iNext+1, $j-$iNext-1)); $iNext = $j + 15 + $cut_r; goto metafield; }
				elsif ($ln =~ /^<media_link>([^<]+)<\/media_link>$/) { ($x, $y, $z) = ("media", "", $1); goto metalink; }
				elsif ($ln =~ /^<link type="([^\"]+)"(?: name="([^\"]+)"|)>([^<]+)<\/link>$/)
				{
					($x, $y, $z) = ($1, $2, $3);
					metalink:
					$z=~s/\&amp;/\&/g;$z=~s/\&\#(\d+);/pack("C",$1)/eg;$z=~s/\&lt;/</g;$z=~s/\&quot;/\"/g;$z=~s/\&gt;/>/g;$z=~s/\&apos;/\'/g;
					$z =~ s/\"/\\\"/g;
				}
				elsif ($ln eq '') { $XMLErr->("Unexpected empty line"); }
				else { $XMLErr->("Unknown XML game line [$ln]"); }
			}
			elsif ($ln =~ /^<game name="([^\"]+)\.(dosz|dosx|chd)">$/ && !$ingame)
			{
				($ingame, @filearr) = ($1);
				$ingame=~s/\&amp;/\&/g;$ingame=~s/\&\#(\d+);/pack("C",$1)/eg;$ingame=~s/\&lt;/</g;$ingame=~s/\&quot;/\"/g;$ingame=~s/\&gt;/>/g;$ingame=~s/\&apos;/\'/g;
				$g = $gamedb{$ingame} || ($gamedb{$ingame} = {});
				if (index($ingame, "\"")   != -1) { $XMLErr->("Double quote contained in game name"); }
				if (index($ingame, "\\")   != -1) { $XMLErr->("Backslash contained in game name"); }
				if (index($ingame, "/")    != -1) { $XMLErr->("Slash contained in game name"); }
				if ($ingame =~ /([\x7F-\xFF])/)   { $XMLErr->("Bad character contained in game name"); }
				if ($ingame =~ /^(The|An|A) /i)   { $XMLErr->("Game name starts with a common article '$1' (should be '... ,$1')"); }
				if ($ingame =~ /:/)               { $XMLErr->("Game name contains a colon"); }
				if ($ingame =~ /~/)               { $XMLErr->("Game name contains a tilde"); }
				if ($g->{files})                  { $XMLErr->("Game entry defined multiple times"); $g = $gamedb{$ingame} = {}; }
				if ($2 ne "dosz") { $g->{ext} = $2; }
				$g->{xml} = $mname;
				if ($lastingame && ("\L$lastingame" cmp "\L$ingame") > 0) { $XMLErr->("Wrongly sorted after '$lastingame'"); }
				$lastingame = $ingame;
			}
			elsif ($inheader == 1) { if ($ln eq "</header>") { $inheader = 2; } }
			elsif ($ln eq "<header>" && !$inheader) { $inheader = 1; }
			elsif ($ln eq '<datafile>') {}
			elsif ($ln eq '</datafile>') { $inheader = 0; }
			elsif ($ln eq '<?xml version="1.0"?>') {}
			elsif ($ln eq '<!DOCTYPE datafile PUBLIC "-//Logiqx//DTD ROM Management Datafile//EN" "http://www.logiqx.com/Dats/datafile.dtd">') {}
			elsif ($ln eq '') { $XMLErr->("Unexpected empty line"); }
			else { $XMLErr->("Unknown XML root line [$ln]"); }
		}
		$numxml++;
	}
	elsif ($mname =~ /([^\/]*)\.dosc$/)
	{
		$ingame = $1;
		if ($gamedb{$ingame}->{dosc}) { Err("DOSC file with same name exists multiple times"); }
		$gamedb{$ingame}->{dosc} = $mname;
		$numdosc++;
	}
	$ingame = undef;
	$mname = undef;
}

foreach $ingame (keys %gamedb)
{
	my $g = $gamedb{$ingame};
	$mname = $g->{xml} || $g->{dosc};
	if ($g->{parent} && !$gamedb{substr($g->{parent}, 0, -5)}->{files}) { Err("Game references non existing parent '$g->{parent}'"); }
	if    ($g->{files})    { }
	elsif ($g->{dosc})     { Err("Lone DOSC without an actual <game> entry existing in the XML"); }
	elsif ($g->{children}) { } # logged above from the child #Err("Game referred via <parent> tag which doesn't exist"); }
	else                   { Err("Unknown error, file list missing"); }
}

print "".($errcount ? "\n" : "")."Finished checking $numxml DAT XMLs and $numdosc DOSC files\n";
exit($errcount ? 1 : 0);

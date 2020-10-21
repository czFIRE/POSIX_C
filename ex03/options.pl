#!/usr/bin/env perl

use v5.30;
use utf8;

use Getopt::Long;

my $options = {};
GetOptions($options, qw(alpha|a beta|b gamma|g=s))
    or exit 1;

say "OPTIONS";
foreach my $key (sort keys %$options) {
    say " * $key is set to value $options->{$key}";
}

say "ARGUMENTS";
foreach my $arg (@ARGV) {
    say " - $arg";
}

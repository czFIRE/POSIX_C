# crc32sum

Programs for creation and verification of CRC32 file hashes.

## Synopsis

```
user$ crc32sum file1 file2 file2 >bundle.crc32
...
user$ crc32check bundle.crc32
```

## Options and Arguments

```
crc32sum [FILES...]
```

* `FILES`: A list of files to compute hashes from. If none provided,
  the program will read from its standard input as well as if `-`
  is passed as one of the arguments.

```
crc32check [BUNDLE]
```

* `BUNDLE`: An optional argument with file that contains bundle
  created by `crc32sum`.

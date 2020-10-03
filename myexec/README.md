# myexec 
Executes program and prints execution time if specified
## Build
```
cc -o myexec myexec.c
```
## Usage:
```
./myexec [-tT] [-qc] program program_options ...
```
### Options:
- **-t**  --  print cpu time
- **-T**  --  print real time
- **-q**  --  quiet mode (suppress executed program's output)
- **-c**  --  count number of lines, words and bytes in output

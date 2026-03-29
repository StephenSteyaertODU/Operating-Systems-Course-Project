## Running the Program

Navigate to the executable directory:

```
cd src/executable
```

### Running with Default Input File

If no argument is provided, the program will automatically use the default input file located in the `data` directory:

```
./scheduler
```

Example output:
```
Loaded 500 processes from data/datafile.txt

Which scheduling algorithm?
1. FIFO
2. SJF
3. BOTH
Enter choice (1, 2, or 3): 1

Results saved to FIFOresults.txt in the CPUSCHED directory.
```

---

### Running with a Custom Input File

You may provide a filename as a command-line argument:

```
./scheduler datafile.txt
```

**Important:**
- The file must be located inside the `data` directory
- Only the filename should be provided (not the full path)

---

### Invalid File Handling

If the file is not found in the `data` directory, the program will display an error and exit:

```
Error: Could not find file: data/datafile.txt
Make sure the file is inside the /data directory.
```

---

### Output File Behavior

When generating output files (`FIFOresults.txt` and/or `SJFresults.txt`), the program automatically handles file creation and overwriting.

- If the output file does not exist, it will be created
- If the output file already exists, it will be cleared and overwritten with the new results


### Output Files

The output file(s) generated depend on the scheduling algorithm selected:

| Option | Algorithm | Output File(s) |
|--------|----------|----------------|
| 1 | FIFO | FIFOresults.txt |
| 2 | SJF | SJFresults.txt |
| 3 | BOTH | FIFOresults.txt and SJFresults.txt |

All output files are saved in the root `CPUSCHED` directory.
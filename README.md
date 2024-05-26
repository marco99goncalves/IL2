
# Garbage Collectors Assignment

## Overview

This assignment explores three different garbage collection algorithms: Mark and Sweep, Mark and Compact, and Copy and Collect. The provided program simulates the behavior of these algorithms in a memory management scenario.

## Running the Program

To run the program, use the following command:

```bash
./mutator [PROBABILITY] [ALGORITHM]
```


### Parameters

- **PROBABILITY**: A floating-point number between 0 and 1 that specifies the probability of object allocation in the simulation.
- **ALGORITHM**: An integer that specifies which garbage collection algorithm to use:
  - `0` - Mark and Sweep
  - `1` - Mark and Compact
  - `2` - Copy and Collect

### Example Usage

1. **Mark and Sweep with a probability of 0.5**:
```bash
./mutator 0.5 0
```

2. **Mark and Compact with a probability of 0.7**:
```bash
./mutator 0.7 1
```

3. **Copy and Collect with a probability of 0.3**:
```bash
./mutator 0.3 2
```

## Compiling the Program

If you need to compile the program, use the following command:

```bash
make -f makefile all
```

Ensure that you have the correct source files and header files in your working directory.

## Additional Notes

- Ensure that the probability value is within the range [0, 1]. Values outside this range will cause undefined behavior.
- Make sure the program has execute permissions. If not, you can add execute permissions using:

```bash
chmod +x mutator
```
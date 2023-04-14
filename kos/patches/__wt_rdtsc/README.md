# KOS `__wt_rdtcs()` function fix

## Brief

The KOS kernel does not allow access to the `CNTVCT_EL0` register from EL0.
MSR instruction throws an exception (causing the program to crash).

## Solution

Use `HalGetCpuTicksRelaxed()` function to get CPU counter.

## Possible issues with solution

The `HalGetCpuTicksRelaxed()` function accesses the **physical** counter
register, not the **virtual** counter register.

This is not an ideal fix because `CNTVCT_EL0 != CNTPCT_EL0`.

The values of these registers are the same under certain [conditions][1], and
these are different registers.

[1]: https://developer.arm.com/documentation/ddi0595/2021-03/AArch64-Registers/CNTVCT-EL0--Counter-timer-Virtual-Count-register


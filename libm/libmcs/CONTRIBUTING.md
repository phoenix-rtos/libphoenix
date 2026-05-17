# Contributing to LibmCS

## How Can I Contribute?

### Reporting Bugs

Report bugs by creating a GitLab issue with the following information:

1. Give the Issue a meaningful title
2. Write a brief description of the issue you have
3. Describe the issue details answering the following questions:
    - What are the steps to reproduce the *bug*?
    - What is the current *bug* behavior?
    - What is the expected *correct* behavior?
    - Relevant logs/screenshots and/or associated data.

We will pick up the issue and continue its workflow.

### Suggesting Enhancements

Suggest enhancements by creating a GitLab issue with the following information:

1. Problem to solve
2. Proposed solution
3. Intended use-case

### Pull Requests

Contribute source code by following the next steps:

1. Fork the repository
2. Create a branch with your code and/or documentation contribution:
    - For the source code format please read the section [Styleguides](#styleguides).
    - Corresponding source code documentation must be added
    - All pipelines must pass
3. Create a merge request on the LibmCS repository

We will pick up the merge request and continue its workflow.

#### Styleguides

To preformat the LibmCS sources we use Astyle. The concrete Astyle options we use are the following:

```sh
> astyle --style=linux --indent=spaces=4 --break-blocks \
--pad-oper --pad-comma --align-pointer=name --add-brackets \
--convert-tabs --pad-header --unpad-paren \
--indent-preproc-block --indent-preproc-define --max-code-length=99
```

After running Astyle some further formatting may be carried out to adjust the overall readability.

As an example you can take the `remquod.c` file in `libm/mathd`. There you
will see how the documentation is written and how the general source code
should look like.

Here is an example `if` structure with comments:

```c
    /* purge off exception values */
    if ((hx >= 0x7ff00000) || (hy >= 0x7ff00000)) { /* x or y not finite */
        if (isnan(x) || isnan(y)) {                 /* x or y is NaN */
            return x + y;
        } else if (hx == 0x7ff00000) {              /* x is infinite */
            return __raise_invalid();
        } else {
            /* No action required */
        }
    } else if ((hy | ly) == 0) {                    /* y = 0 */
        return __raise_invalid();
    } else {
        /* No action required */
    }
```

And here is a `for` loop:

```c
    /* compute q[0],q[1],...q[jk] */
    for (i = 0; i <= jk; i++) {
        for (j = 0, fw = 0.0; j <= jx; j++) {
            fw += x[j] * f[jx + i - j];
        }

        q[i] = fw;
    }
```

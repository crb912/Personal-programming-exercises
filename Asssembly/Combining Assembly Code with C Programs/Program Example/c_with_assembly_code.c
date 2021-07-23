#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// signed multiply, error version in C code; it checks wrong property.
int tmult_ok_c(long, long, long *);
// the function define in handwritten assembly-code. fix version
int tmult_ok_asm(long, long, long *);

// unsigned multiply
int umult_ok_c(unsigned long, unsigned long, unsigned long *);
// the function define in handwritten assembly-code. fix version
int umult_ok_asm(unsigned long, unsigned long, unsigned long *);

int tmulk_ok_asm_v1(long, long, long *);
int tmulk_ok_asm_v2(long, long, long *);

// ---------------------------------------------------------------------------
int tmult_ok_int_op(int x, int y) {
    int64_t p = (int64_t)x*y;
    return p == (int) p;
}

int tmult_ok_long_op(long x, long y, long *dest) {
    long p = x*y;
    *dest = p;
    return !x || p/x == y;
}

void check_by_additional_op(){
    printf("Determine whether a multiplication has overflowed by additional op.\n\n");
    int i1 = tmult_ok_int_op(INT_MAX, 1);
    printf("OK! (INT_MAX, 1) expect return 1, we got: %d\n", i1);
    int i2 = tmult_ok_int_op(INT_MAX, 2);
    printf("OK! (INT_MAX, 2) expect return 0, we got: %d\n", i2);
    int i3 = tmult_ok_int_op(INT_MAX, 3);
    printf("OK! (INT_MAX, 3) expect return 0, we got: %d\n\n", i3);

    long var;
    int c1 = tmult_ok_long_op(LONG_MAX, 1, &var);
    printf("OK! (LONG_MAX, 1) expect return 1, we got: %d\n", c1);
    int c2 = tmult_ok_long_op(LONG_MAX, 2, &var);
    printf("OK! (LONG_MAX, 2) expect return 0, we got: %d\n", c2);
    int c3 = tmult_ok_long_op(LONG_MAX, 3, &var);
    printf("OK! (LONG_MAX, 3) expect return 0, we got: %d\n\n", c3);
}

// C example code
int tmult_ok_c(long x, long y, long *dest) {
    long p = x*y;
    *dest = p;
    return p > 0;
}

int umult_ok_c(unsigned long x, unsigned long y, unsigned long *dest) {
    unsigned long p = x*y;
    *dest = p;
    return p > 0;
}

// handwritten assembly-code functions
void hand_write_asm(){
    printf("Determine whether a multiplication has overflowed by handwritten asm function.\n\n");
    long var;
    unsigned long var2 = 0;
    int x1 = tmult_ok_c(LONG_MAX, 1, &var);
    printf("OK! (LONG_MAX, 1) expect return 1, we got: %d\n", x1);
    int x2 = tmult_ok_c(LONG_MAX, 2, &var);
    printf("Example Error! (LONG_MAX, 2) expect return 0 ,but we got: %d\n", x2);
    int z = umult_ok_c(ULONG_MAX, 2, &var2);
    printf("Example Error! (ULONG_MAX, 2)expect return 0 ,but we got: %d\n\n", z);

    int y = tmult_ok_asm(LONG_MAX, 2, &var);
    printf("OK! (LONG_MAX, 2) expect return 0, we got: %d\n", y);
    int t = umult_ok_asm(ULONG_MAX, 2, &var2);
    printf("OK! (ULONG_MAX, 2)expect return 0, but we got: %d\n", t);
}

// ----------------------------------------------------------------------------
/*
 * The function saves the value of %rbx (callee saved) on the stack and restores
 * it at the end. Since this register is a callee-saved register, and we have
 * indicated that our code will overwrite its low-order bytes (register %bl),
 * GCC takes the necessary steps to preserve its value.
 * */

int tmulk_ok_asm_v1(long x, long y, long* dest)
{
    int result;

    *dest = x *y ;
    asm volatile("setae %%bl  \n\t"
        "movzbl %%bl, %[val]"
    : [val] "=r" (result)
    :
    : "%bl"
    );
    // if we add the statement, GCC won't produces wrong optimization
    //printf("asm x100(signed):%d, use %%bl \n", result);
    return result;
}

/* As s further refinement, we can simplify the code even more and make use of
 * GCC's ability to work with different data types. This simplified form avoids
 * the need for us to make use of a specific register, and hence we need not
 * specify any overwritten registers.
*/
int tmulk_ok_asm_v2(long x, long y, long *dest){
    unsigned char bresult;
    *dest = x * y;

    asm("setae %[b]"
    : [b] "=r" (bresult)
    );
    return (int)(bresult);
}

/*Including the imulq instruction as part of the assembly code.
 * It can be argued that this approach will be more robust across different version
 * of GCC and optimization levels, since it will guarantee that the correct form
 * of multiplication is being used.
 *
 * This function will guarantee that the correct in the case of
 * the optimization flag with "-O2" or "-O1".
 * */
int tmulk_ok_asm_v3(long x, long y, long* dest)
{
    unsigned char bresult;

    asm("imulq %[x], %[y] \n\t"
        "movq %[y], %[p] \n\t"
        "setae %[b]"
    : [b] "=r" (bresult),[p] "=m" (*dest)
    : [x] "r" (x), [y] "r" (y)   /* Inputs */
    :
    );
    return (int)bresult;
}

int umult_ok(unsigned long x, unsigned long y, unsigned long *dest){
    unsigned char bresult;
    asm("movq %[x], %%rax \n\t"
        "mulq %[y] \n\t"
        "movq %%rax, %[p] \n\t"
        "setae %[b]"
    : [p] "=r" (*dest) ,[b] "=r" (bresult)
    : [x] "r" (x), [y] "r" (y)
    : "%rax", "%rdx"
    );
    return (int)(bresult);
}


void inline_asm(){
    printf("Determine whether a multiplication has overflowed by inline assembly\n");
    printf("Warning: Should test with different levels of optimization\n");
    long var;
    /* FIXME: Unfortunately, the generated code does not work as desired.
     * GCC has its own ideas of code generation. Invoking GCC with option -O1
     * or higher (e.g., -O2 or 0O3) will cause it use wrong values.
     * More precisely, the value of x is not returned by the function tmulk_ok_asm_v1
     * but is given.
     * See: https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
     * GCC’s optimizers sometimes discard asm statements if they determine there
     * is no need for the output variables. Also, the optimizers may move code
     * out of loops if they believe that the code will always return the same
     * result (i.e. none of its input values change between calls).
     * */
    int x1 = tmulk_ok_asm_v1(LONG_MAX, 1, &var);
    printf("ASM V1 OK! (LONG_MAX, 1) expect return 1, we got: %d\n", x1);
    x1 = tmulk_ok_asm_v1(LONG_MAX, 2, &var);
    printf("ASM V1 OK! (LONG_MAX, 2) expect return 0, we got: %d\n", x1);

    int x2 = tmulk_ok_asm_v2(LONG_MAX, 1, &var);
    // ok! we got 0. return 0 when the multiplication overflow.
    printf("ASM V2 OK! (LONG_MAX, 1) expect return 1, we got: %d\n", x2);
    x2 = tmulk_ok_asm_v2(LONG_MAX, 2, &var);
    printf("ASM V2 OK! (LONG_MAX, 2) expect return 0, we got: %d\n", x2);

    int x3 = tmulk_ok_asm_v3(LONG_MAX, 1, &var);
    // ok! we got 0. return 0 when the multiplication overflow.
    printf("ASM V2 OK! (LONG_MAX, 1) expect return 1, we got: %d\n", x3);
    x3 = tmulk_ok_asm_v3(LONG_MAX, 2, &var);
    // ok! we got 0. return 0 when the multiplication overflow.
    printf("ASM V3 OK! (LONG_MAX, 2) expect return 0, we got: %d\n\n", x3);

    unsigned long var2;
    int z = umult_ok(ULONG_MAX, 1, &var2);
    printf("ASM Umulk OK! (ULONG_MAX, 1) expect return 1, we got: %d\n", z);
    z = umult_ok(ULONG_MAX, 2, &var2);
    // ok! we got 0. return 0 when the multiplication overflow
    printf("ASM Umulk OK! (ULONG_MAX, 2) expect return 0, we got: %d\n", z);
}

int main(){
    check_by_additional_op();
    hand_write_asm();
    inline_asm();
    return 0;
}
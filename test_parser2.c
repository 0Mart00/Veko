#include <stdio.h>
#include <string.h>

int main() {
    char obj_name[64], class_name[64];
    char line1[] = "x = 5";
    char line2[] = "x = MyClass()";
    
    // Check if line ends with ()
    int len1 = strlen(line1);
    int len2 = strlen(line2);
    
    printf("Line 1: '%s' ends with ()? %d\n", line1, 
           (len1 >= 2 && line1[len1-1] == ')' && line1[len1-2] == '('));
    printf("Line 2: '%s' ends with ()? %d\n", line2,
           (len2 >= 2 && line2[len2-1] == ')' && line2[len2-2] == '('));
    
    // Better pattern: check for () explicitly
    if (sscanf(line2, "%s = %[^(]()", obj_name, class_name) == 2) {
        // Additional check: does the line end with ()?
        int len = strlen(line2);
        if (len >= 2 && line2[len-1] == ')' && line2[len-2] == '(') {
            printf("Valid object creation: %s = %s()\n", obj_name, class_name);
        }
    }
    
    return 0;
}

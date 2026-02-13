#include <stdio.h>
#include <string.h>

int main() {
    char obj_name[64], class_name[64];
    
    // Test 1: x = 5
    int result1 = sscanf("x = 5", "%s = %[^(]()", obj_name, class_name);
    printf("Test 1 'x = 5': result=%d, obj='%s', class='%s'\n", result1, obj_name, class_name);
    
    // Test 2: x = MyClass()
    int result2 = sscanf("x = MyClass()", "%s = %[^(]()", obj_name, class_name);
    printf("Test 2 'x = MyClass()': result=%d, obj='%s', class='%s'\n", result2, obj_name, class_name);
    
    // Test 3: x = 5 with better pattern
    char target[64], value[64];
    int result3 = sscanf("x = 5", "%s = %s", target, value);
    printf("Test 3 'x = 5' simple: result=%d, target='%s', value='%s'\n", result3, target, value);
    
    return 0;
}

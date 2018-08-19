Java
===========
[TOC]

Java include mechanism
===========

Java is a dynamic language where the `import` statement
is just used to name classes by there unqualified name.

Examples
-----------
```java
import com.foo.bar;
import static com.foo.bar
```

Considerations
-----------
Due to the fact described above, there is no use case for java. Or is there one?

Assuming, that the import statements are analyzed, the result doesn't say something
meaningful if a class is used or not. This is the same situation to
C#.

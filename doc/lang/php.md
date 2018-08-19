PHP
===========
[TOC]

PHP include mechanism
===========

See
 - [https://secure.php.net/manual/en/language.namespaces.importing.php](https://secure.php.net/manual/en/language.namespaces.importing.php)

The `use` statement is done at block scope.


Examples
-----------
```php
use Foo;
use Foo\Bar;
use function Foo\Bar;
use function Foo\Bar as B;
use const Foo\Bar\C;
use Foo\{Bar1,Bar2,Bar3,Bar4 as BBBB};
use const Foo\Bar\{C1,C2,C3,C4,C5};

```

Considerations
-----------

Java-Script
===========
[TOC]

Java-Script include mechanism
===========
There exists three ways to import JS modules:
 - ES5 via CommonJS
 - ES5 via AMD / RequireJS
 - ES6 via EXMAScript 6

See
 - [http://exploringjs.com/es6/ch_modules.html#sec_modules-in-javascript](http://exploringjs.com/es6/ch_modules.html#sec_modules-in-javascript)
 - [https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/import](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/import)


Examples
-----------


CommonJS
```javascript
var foobar = require('./foobar').foobar,
var mod_foo = require('./foo');
var mod_bar = require('./bar');

```



AMD
```javascript

require(['foo', 'bar'], 
   function ( foo, bar ) {
        foo.doSomething();
   }
);

require(['app/myModule'], 


);



// AMD Modules using curl.js
curl(['app/myModule.js'], 


);

```

ES6
```javascript
import {foo} from 'foo';
import * as f from "foo";
import {b1,b2} from "bar"
var x = import(foo);
import * as myModule from '/foo/bar/a-module.js';
import {reallyReallyLongModuleExportName as name} from '/a/path/to/foo/bar/script.js';
import { abs as a, bcd as b} from 'a/b/foo.js';
import myDefaults from 'a/b/foo.js'
import myDefaults, * as aModule from 'a/b/foo.js'
import myDefaults, {foo,bar} from 'a/b/foo.js'

import('/a/b/c/d/foo/bar.js')
   .then((module)=>{
      // further code
   });
let module = await import('a/b/c/foo/bar.js');
```


Considerations
-----------
It would make sense to consider only import statements starting with the 6th edition
of ECMAScript.
In addition, iw would also make sense to parse not only `*.js` files,
also `*.html` files may contain javascript code.


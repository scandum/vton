VTON (Virtual Terminal Object Notation)
---------------------------------------

The VTON encodings draw inspiration from UTF-8, VT100, and TELNET.

VTON uses Base252 encoding for binary data which is described here:

https://github.com/scandum/base252

While Base252 stands by itself there is an annoying issue in computer science
that has yet to be properly addressed.

It wasn't until the mid 90s that computer scientists realized the need for a
universal structured data format, and one of the answers to this need was XML
which was created in 1996 and JSON which was created in 2013.

Both XML and JSON have strengths and weaknesses. The goal of VTON (Virtual
Terminal Object Notation) is to create an object notation that is compact,
well suited for machine to machine communication, and semi human readable.

While JSON is a step forward compared to XML it has issues with verbosity,
escapability, and complexity. It is verbose because it is typed while
typing is often irrelevant, typing also adds unnecessary complexity. Read
up on data types for more information.

JSON further adds various escaping requirements to aid in human readability.
However, the utility of this is questionable whenever the chance of a human
actually looking at a data exchange is less than 0.0001%, while those that
do look at machine 2 machine data exchanges are unlikely to be confused by the
inclusion of escape characters. Not to mention they could use a special
viewer that displays escape characters in a readable manner.

JSON adds further rules and restrictions that result in JSON parsers being
complex pieces of software, which in turn increases the chance of accidental
bugs.

To address these issues VTON is typeless, simpel and logical. VTON knows the
following 6 special symbols in the escape character range.

| Code | Hex  | Name | Symbol |
|----- | ---- | ---- | ------ |
|   1  | 0x01 | VTON_VARIABLE    | $ |
|   2  | 0x02 | VTON_VALUE       | = |
|   3  | 0x03 | VTON_TABLE_OPEN  | { |
|   4  | 0x04 | VTON_TABLE_CLOSE | } |
|   5  | 0x05 | VTON_ARRAY_OPEN  | [ |
|   6  | 0x06 | VTON_ARRAY_CLOSE | ] |


VTON value assignment:
---------------------
A VTON value assignment looks as following:
```
1 VARIABLE_NAME 2 VALUE
```
1 and 2 are ASCII character 1 and 2. VARIABLE and VALUE are
UTF-8 encoded strings. To make things more readable I will also provide the
same notation using symbols, which looks as following:
```
$VARIABLE_NAME : VALUE
```
VTON table assignment:
----------------------
A VTON table assignment looks as following:
```
1 TABLE_NAME 2 3 1 VARIABLE_NAME 2 VALUE 1 VARIABLE_NAME 2 VALUE 4
```
```php
$TABLE_NAME :
{
        $VARIABLE_NAME : VALUE
        $VARIABLE_NAME : VALUE
}
```
A variable assignment must be terminated by either VTON_VALUE, VTON_TABLE_OPEN,
or VTON_ARRAY_OPEN.

There is no concept of a comma which is not necessary because it's easy to keep
track of key/value pairs. Multiple nests can be created.
```php
$TABLE_NAME :
{
        $VARIABLE_NAME : VALUE
        $TABLE_NAME :
        {
                VARIABLE : VALUE
                VARIABLE : VALUE
        }
}
```
VTON array assignment
---------------------
A VTON array assignment looks as following:
```
1 ARRAY_NAME 2 5 2 VALUE1 2 VALUE2 2 VALUE3 6
```
```php
$ARRAY_NAME : [ : VALUE1 : VALUE2 : VALUE3 ]
```
Since there is no comma the VTON_VALUE code is used to separate array values. Multiple nests can be created.
```php
$ARRAY_NAME :
[
        : [ : VALUE1 : VALUE1 ]
        : [ : VALUE1 : VALUE2 ]
]
```
Variable names
--------------
Variable names ought to start with a letter and exclusively contain letters,
numbers, and underscores. They should also be case sensitive. While VTON is
flexible that doesn't mean that the programming languages interacting with
VTON data are.

Compatibility
-------------
VTON is easy to convert to JSON and back, with the exception that data cannot
be typed, so when converting from VTON to JSON everything needs to be
converted to string notation.

Displayability
--------------
A VTON viewer should be relatively simple to create.

Base252 + VTON
--------------
By combining Base252 and VTON you end up with the ability to incorporate
binary data that is easy to encode and decode.

VTON overhead
-------------
Compared to Base252 VTON changes the average overhead from 1.7% to 4%. The
best case remains 0% for ASCII and UTF-8 encoded text.

The overhead of VTON compared to JSON is trickier to calculate, except that
the VTON overhead is less in any given scenario.

While JSON is readable, it is only readable by including spacing which further
increases the data overhead. One could argue that spacing could be removed,
but then the argument that JSON is more readable than VTON is moot.

VTON parser
-----------
In order to parse VTON you need a VTON parser. While it doesn't take much code
to write a VTON parser the concept can be difficult to wrap your head around.

I'll try to publish my VTON parser in the near future.

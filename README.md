VTON (Virtual Terminal Object Notation)
---------------------------------------

The VTON encoding draws inspiration from UTF-8, VT100, and TELNET. It provides
a typeless object notation system.

Background
----------

It wasn't until the mid 90s that computer scientists realized the need for a
universal structured data format, and one of the answers to this need was XML
which was created in 1996 and JSON which was created in 2002.

Both XML and JSON have strengths and weaknesses. The goal of VTON (Virtual
Terminal Object Notation) is to create an object notation that is compact,
well suited for machine to machine communication, and semi human readable.

While JSON is a step forward compared to XML it has issues with verbosity,
escapability, and complexity. It is verbose because it is typed while
typing is often irrelevant, typing also adds unnecessary complexity. Read
up on data types for more information.

JSON adds various escaping requirements to aid in human readability.
However, the utility of this is questionable whenever the chance of a human
actually looking at a data exchange is less than 0.0001%, while those that
do look at machine 2 machine data exchanges are unlikely to be confused by the
inclusion of escape characters. Not to mention they could use a special
viewer that displays escape characters in a readable manner.

JSON adds further rules and restrictions that result in JSON parsers being
complex pieces of software, which in turn increases the chance of accidental
bugs and inconsistent behavior between parsers. JSON also contains restrictions
that could be considered wholly unnecessary, like the requirement for strings
to encode valid unicode codepoints.

VTON tries to address these issues.

Definition
----------

VTON knows the following 6 special symbols outside the UTF-8 character range.

| Code | Hex  | Name | Symbol |
|----- | ---- | ---- | ------ |
| 249  | 0xF9 | VTON_VARIABLE    | $ |
| 250  | 0xFA | VTON_VALUE       | : |
| 251  | 0xFB | VTON_TABLE_OPEN  | { |
| 252  | 0xFC | VTON_TABLE_CLOSE | } |
| 253  | 0xFD | VTON_ARRAY_OPEN  | [ |
| 254  | 0xFE | VTON_ARRAY_CLOSE | ] |


VTON value assignment:
---------------------
A VTON value assignment looks as following:
```
249 VARIABLE_NAME 250 VALUE
```
249 and 250 are byte characters with the corresponding value. VARIABLE and VALUE are
strings.

To make things more readable I will also provide the same notation using
symbols most programmers are familiar with, which looks as following:
```php
$VARIABLE_NAME : VALUE
```
VTON table assignment:
----------------------
A VTON table assignment looks as following.
```
249 TABLE_NAME 250
251
        249 VARIABLE_NAME 250 VALUE
        249 VARIABLE_NAME 250 VALUE
252
```
```php
$TABLE_NAME :
{
        $VARIABLE_NAME : VALUE
        $VARIABLE_NAME : VALUE
}
```
There is no concept of a comma, this because it's easy to keep track of key/value
pairs. Multiple nests can be created.
```
249 TABLE_NAME 250
251
        249 VARIABLE_NAME 250 VALUE
        249 TABLE_NAME 250
        251
                249 VARIABLE_NAME1 250 VALUE
                249 VARIABLE_NAME2 250 VALUE
        252
252
```
```php
$TABLE_NAME :
{
        $VARIABLE_NAME : VALUE
        $TABLE_NAME :
        {
                $VARIABLE_NAME1 : VALUE
                $VARIABLE_NAME2 : VALUE
        }
}
```
VTON array assignment
---------------------
A VTON array assignment looks as following:
```
249 ARRAY_NAME 250 253 250 VALUE1 250 VALUE2 250 VALUE3 254
```
```php
$ARRAY_NAME : [ : VALUE1 : VALUE2 : VALUE3 ]
```
Since there is no comma the VTON_VALUE code is used to separate array values. Multiple nests can be created.
```
249 ARRAY_NAME 250
253
        250 253 250 VALUE1 250 VALUE2 254
        250 253 250 VALUE1 250 VALUE2 254
254
```
```php
$ARRAY_NAME :
[
        : [ : VALUE1 : VALUE2 ]
        : [ : VALUE1 : VALUE2 ]
]
```
Variable names
--------------
Variable names ought to start with a letter and exclusively contain letters,
numbers, and underscores. They should also be case sensitive. While VTON is
flexible that doesn't mean that the programming languages interacting with
VTON data are.

Values
------
Values should not contain a 0 byte, or byte values between 249 and 254. VTON
does not define a default escaping mechanism because all UTF-8 sequences
are valid. Base252 is suggested for embedding other encodings as well as binary
data.

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

https://github.com/scandum/base252

VTON overhead
-------------
Base252 encodes binary data with an average overhead of 1.7%, when using Base252
in conjunction with VTON you also need to escape byte values in the 249 to 254 range,
which brings the average overhead to 4%.

VTON parser
-----------
In order to parse VTON you need a VTON parser. While it doesn't take much code
to write a VTON parser, nesting can be difficult to wrap your head around.

The vton.c file contains two utility functions to change VTON to JSON and back.

VTON (Virtual Terminal Object Notation) and Base252
---------------------------------------------------

The VTON and Base252 encodings draw inspiration from UTF-8, VT100, TELNET, and
seeks to create a flexible and powerful universal structured base encoding.

Base252
-------

| Code |  Hex | Name    | Escape |
|----- | ---- | ------  | ------ |
|  0   | 0x00 | NUL     | 0x00 0x80 |
| 192  | 0xC0 | ESC_0   | 0xF7 0x80 |
| 193  | 0xC1 | ESC_64  | 0xF7 0x81 |
| 247  | 0xF7 | ESC_192 | 0xF7 0xB7 |

Translating binary data to Base252
----------------------------------

The first step is to convert the data into strings. This is done by translating
each NUL byte 0x00 to 0xC0 0x80. This is the type of escaping allowed by UTF-8
and anyone not familiar with the subject matter should read up on UTF-8 for a
full explanation.

Subsequently each instance of 0xC0 needs to be translated as well. If we
were to stick with UTF-8 0xC0 could be translated to 0xC3, but this would
result in a lot of doubling for languages that use the unicode equivalent
of the extended characters of ISO 8859-1.

So instead each instance of 0xC0 is translated to 0xF7 0x80. And in turn
each instance of 0xF7 needs to be translated to 0xF7 0xB7.

Escaping
--------

Earlier we saw the escaping of 0x00 to 0xC0 0x80. However, in many cases
programming languages have special characters that pose processing or security
issues when they are not escaped. The \ character comes to mind, but in
theory any character can be a special character that needs to be escaped.

In order to escape any ASCII character Base252 reserves 0xC0, 0xC1 for this
purpose. The math to escape is simple and is semi UTF-8 compatible.
```
0xC0 + char / 64
0x80 + char % 64
```
In turn 0xC0 and 0xC1 need to be escaped using.
```
0xF7
0x80 + char % 64
```
Subsequently we can optionally escape character 1 through 127, and character
194 through 255 with the exception of character 247 aka 0xF7.

Characters 128 through 191 cannot be escaped. Translating data to Base252 is
relatively easy.
```
        for (cnt = 0 ; cnt < size ; cnt++)
        {
                switch (input[cnt])
                {
                        case 0:
                                *output++ = 192;
                                *output++ = 128 + input[cnt] % 64;
                                break;

                        case '\\':
                                *output++ = 192 + input[cnt] / 64;
                                *output++ = 128 + input[cnt] % 64;
                                break;

                        case 192:
                        case 193:
                        case 247:
                                *output++ = 247;
                                *output++ = 128 + input[cnt] % 64;
                                break;

                        default:
                                *output++ = input[cnt];
                                break;
                }
        }
        *output++ = 0;
```

Translating Base252 data back to its original format is equally easy.
```
        for (cnt = 0 ; cnt < size ; cnt++)
        {
                switch (input[cnt])
                {
                        case 0xC0:
                                cnt++;
                                *output++ = 0 + input[cnt] % 128;
                                break;

                        case 0xC1:
                                cnt++;
                                *output++ = 64 + input[cnt] % 64;
                                break;

                        case 0xF7:
                                cnt++;
                                *output++ = 192 + input[cnt] % 64;
                                break;

                        default:
                                *output++ = input[cnt];
                                break;
                }
        }
        *output++ = 0;
```

Overhead
--------
After this conversion we have a best case overhead of 0% (notably when turning
ASCII or UTF-8 into Base252) and a worst case overhead of 100%. The average
case should be an overhead of 1.7% and this should also be the typical case
for compressed data, like images.


VTON
----
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
actually looking at a data exchange is less than 0.0001%, and those that
actually do look at a data exchanged are unlikely to be confused by the
inclusion of escape characters. Not to mention they could use a special
viewer that displays escape characters in a readable manner.

JSON adds further rules and restrictions that result in JSON parsers being
complex pieces of software, which in turn increases the chance of accidental
bugs.

To address these issues VTON is typeless, simpel and logical. VTON knows the
following 6 special symbols.

| Code | Hex  | Name | Symbol |
|----- | ---- | ---- | ------ |
| 248  | 0xF8 | VTON_VARIABLE    | $ |
| 249  | 0xF9 | VTON_VALUE       | = |
| 250  | 0xFA | VTON_TABLE_OPEN  | } |
| 251  | 0xFB | VTON_TABLE_CLOSE | } |
| 252  | 0xFC | VTON_ARRAY_OPEN  | [ |
| 253  | 0xFD | VTON_ARRAY_CLOSE | ] |


VTON value assignment:
---------------------
A VTON value assignment looks as following:
```
248 VARIABLE_NAME 249 VALUE
```
248 and 249 are extended ASCII character 248 and 249. VARIABLE and VALUE are
UTF-8 encoded strings. To make things more readable I will also provide the
same notation using symbols, which looks as following:
```
$VARIABLE_NAME = VALUE
```
VTON table assignment:
----------------------
A VTON table assignment looks as following:
```
248 TABLE_NAME 250 248 VARIABLE_NAME 249 VALUE 248 VARIABLE_NAME 249 VALUE 251
```
```
$TABLE_NAME
{
        $VARIABLE_NAME = VALUE
        $VARIABLE_NAME = VALUE
}
```
A variable assignment must be terminated by either VTON_VALUE, VTON_TABLE_OPEN,
or VTON_ARRAY_OPEN.

There is no concept of a comma which is not necessary because it's easy to keep
track of key/value pairs. Multiple nests can be created.
```
$TABLE_NAME
{
        $VARIABLE_NAME = VALUE
        $TABLE_NAME
        {
                VARIABLE = VALUE
                VARIABLE = VALUE
        }
}
```
VTON array assignment
---------------------
A VTON array assignment looks as following:
```
248 ARRAY_NAME 252 249 VALUE1 249 VALUE2 249 VALUE3 253
```
```
$ARRAY_NAME
[
        = VALUE
        = VALUE
        = VALUE
]
```
Since there is no comma the VTON_VALUE code is used to separate array values. Multiple nests can be created.
```
$ARRAY_NAME
[
        [
                = VALUE
                = VALUE
        ]
        [
                = VALUE
                = VALUE
        ]
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
A VTON viewer should be simple to create. If VTON becomes popular enough
UTF-8 viewers might display VTON codes using specially assigned symbols
instead of error symbols, since all VTON codes are illegal UTF-8 codes.

Base252 + VTON
--------------
By combining Base252 and VTON you end up with the ability to incorporate
binary data that is easy to encode and decode.

VTON overhead
-------------
Compared to Base252 VTON changes the average overhead from 1.7% to 4%. The
best case remains 100% for ASCII and UTF-8 encoded text.

The overhead of VTON compared to JSON is trickier to calculate, except that
the VTON overhead is less in any given scenario.

For binary data you are pretty much forced to use Base64 with JSON which
means a 33.3% data overhead as well as a computational overhead.

While JSON is readable, it is only readable by including spacing which further
increases the data overhead. One could argue that spacing could be removed,
but then any argument that JSON is more readable than VTON is moot.

VTON parser
-----------
In order to parse VTON you need a VTON parser. While it doesn't take much code
to write a VTON parser the concept can be difficult to wrap your head around.

I'll try to publish my VTON parser in a couple of days.

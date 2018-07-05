# `pipe.c`

|Name|Source|Destination|Updates position|Returns read|Fatal on EOI|
|---|---|---|---|---|---|
|`accept`|Buffer, then Input|Output|Yes|Yes|Yes|
|`skip`|Buffer, then Input|-|Yes|N|Yes|
|`peek`|Buffer, then Input|Buffer|N|Yes|Yes|
|`write`|Parameter|Output|N|N|-|

# Processing module

There are many structs and associated methods in the `crate::proc` module relating to processing, such as checkpoints and ranges. They often need to work with the code, so methods could be declared on `Processor` or themselves. For the sake of reducing the amount of code/methods in `Processor`, methods should always be declared on the specific struct, even if it appears awkward. This makes code easier to comprehend and work with and avoids too many verbose (to avoid name clashes) or ambiguous method names on `Processor`.

Since Rust does not make it easy to hold dangling references, many methods that require `Processor` will require passing it in every time. 

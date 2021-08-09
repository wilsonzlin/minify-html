# c14n

Parse HTML from stdin and write a canonical HTML document to stdout. Useful to preprocess documents for diffing:

- Sort attributes by name.
- Decode all entities, then re-encode only special characters consistently.
- Make tag and attribute names lowercase.

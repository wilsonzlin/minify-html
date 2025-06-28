#!/bin/bash

# Configuration paths
RUST_CFG_FILE="../minify-html/src/cfg/mod.rs"
JAVA_CONFIG_FILE="src/main/java/in/wilsonl/minifyhtml/Configuration.java"
RUST_LIB_FILE="src/main/rust/lib.rs"

# Function to convert snake_case to camelCase
snake_to_camel() {
    echo "$1" | sed -r 's/_([a-z])/\U\1/g'
}

# Function to capitalize first letter
capitalize() {
    echo "$(tr '[:lower:]' '[:upper:]' <<< ${1:0:1})${1:1}"
}

echo "Extracting fields from Rust configuration..."

# Extract field names using grep and sed
rust_fields=$(grep -oP '^\s*pub \K[a-zA-Z0-9_]+(?=: bool,?\s*$)' "$RUST_CFG_FILE")

if [ -z "$rust_fields" ]; then
    echo "No boolean fields found in $RUST_CFG_FILE"
    exit 1
fi

echo "Found fields:"
while IFS= read -r field; do
    echo "  - $field"
done <<< "$rust_fields"

# Generate Java fields
echo "Generating Java fields..."
java_fields=""
while IFS= read -r field; do
    camel_field=$(snake_to_camel "$field")
    java_fields="$java_fields  boolean $camel_field;\n"
done <<< "$rust_fields"

# Update Java configuration file between markers
echo "Updating Java configuration file..."
temp_java=$(mktemp)
awk -v fields="$java_fields" '
    /\/\* BEGIN FIELD NAMES \*\// {
        print $0
        printf "%s", fields
        skip=1
        next
    }
    /\/\* END FIELD NAMES \*\// {
        print $0
        skip=0
        next
    }
    !skip {
        print
    }
' "$JAVA_CONFIG_FILE" > "$temp_java"
mv "$temp_java" "$JAVA_CONFIG_FILE"

# Generate Rust mappings
echo "Generating Rust JNI mappings..."
rust_mappings=""

while IFS= read -r field; do
    camel_field=$(snake_to_camel "$field")
    getter_name="get$(capitalize "$camel_field")"

    mapping="    $field: env.call_method(*obj, \"$getter_name\", \"()Z\", &[]).unwrap().z().unwrap(),"

    rust_mappings="$rust_mappings$mapping\n"
done <<< "$rust_fields"

# Update Rust lib file between markers
echo "Updating Rust library file..."
temp_rust=$(mktemp)
awk -v mappings="$rust_mappings" '
    /\/\/ BEGIN CONFIGURATION FIELDS/ {
        print $0
        printf "%s", mappings
        skip=1
        next
    }
    /\/\/ END CONFIGURATION FIELDS/ {
        print $0
        skip=0
        next
    }
    !skip {
        print
    }
' "$RUST_LIB_FILE" > "$temp_rust"
mv "$temp_rust" "$RUST_LIB_FILE"

echo "Code generation completed!"
echo "Updated files:"
echo "  - $JAVA_CONFIG_FILE"
echo "  - $RUST_LIB_FILE"

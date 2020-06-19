require 'rake'

Gem::Specification.new do |spec|
  spec.name          = "hyperbuild"
  spec.version       = "0.1.1"
  spec.authors       = ["Wilson Lin"]
  spec.email         = ["code@wilsonl.in"]
  spec.license       = "MIT"
  spec.files         = FileList["lib/*", "README.md"].to_a
  spec.summary       = "Fast one-pass in-place HTML minifier written in Rust with context-aware whitespace handling"
  spec.homepage      = "https://github.com/wilsonzlin/hyperbuild"

  spec.require_paths = ["lib"]

  spec.add_dependency "fiddle", "~> 1.0"
end

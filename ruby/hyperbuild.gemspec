require 'rake'

Gem::Specification.new do |spec|
  spec.name          = "hyperbuild"
  spec.version       = "0.2.4"
  spec.authors       = ["Wilson Lin"]
  spec.email         = ["code@wilsonl.in"]
  spec.license       = "MIT"
  spec.files         = FileList["lib/*", "README.md"].to_a
  spec.summary       = "Fast allocation-less HTML minifier with smart whitespace handling"
  spec.homepage      = "https://github.com/wilsonzlin/hyperbuild"

  spec.require_paths = ["lib"]

  spec.add_dependency "fiddle", "~> 1.0"
end

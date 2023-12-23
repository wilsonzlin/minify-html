Gem::Specification.new do |spec|
  spec.name          = "minify_html"
  spec.version       = "0.11.2"
  spec.authors       = ["Wilson Lin"]
  spec.email         = ["code@wilsonl.in"]
  spec.license       = "MIT"
  spec.files         = Dir["lib/**", "ext/**", "README.md"]
  spec.summary       = "Extremely fast and smart HTML + JS + CSS minifier"
  spec.homepage      = "https://github.com/wilsonzlin/minify-html"

  spec.require_paths = ["lib"]

  spec.extensions = ["ext/minify_html/extconf.rb"]

  # needed until rubygems supports Rust support is out of beta
  spec.add_dependency "rb_sys", "~> 0.9.39"
  # only needed when developing or packaging your gem
  spec.add_development_dependency("rake", "~> 13.0")
  spec.add_development_dependency "rake-compiler", "~> 1.2"
end

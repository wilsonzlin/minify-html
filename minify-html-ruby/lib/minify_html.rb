# frozen_string_literal: true

begin
  /(?<ruby_version>\d+\.\d+)/ =~ RUBY_VERSION
  require_relative "#{ruby_version}/minify_html"
rescue LoadError
  require_relative "minify_html.so"
end

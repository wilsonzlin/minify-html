require 'fiddle'

class MinifyHtmlLoader
  def self.operating_system
    case RbConfig::CONFIG['host_os'].downcase
    when /linux|bsd|solaris/ then 'linux'
    when /darwin/ then 'macos'
    when /mingw|mswin/ then 'windows'
    else 'unknown'
    end
  end

  def self.lib_path
    File.join(__dir__, [operating_system, '-ruby', RUBY_VERSION.split('.')[0...-1].join('.')].join(''))
  end
end

Fiddle::Function.new(Fiddle.dlopen(MinifyHtmlLoader.lib_path)['Init_minify_html'], [], Fiddle::TYPE_VOIDP).call

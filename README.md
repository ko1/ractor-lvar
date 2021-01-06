# Ractor::LVar

NOTE: This gem is experimental and I don't guarantee to continue maintenance of this gem.

More flexible Ractor-local variable object.

```ruby
require 'ractor/lvar'

V = Ractor::LVar.new{ 42 }

r = Ractor.new do
  V.ractor_local_value *= 10
  V.ractor_local_value
end

p [V.ractor_local_value, r.take]
#=> [42, 420]
```

## Installation

You need Ruby 3.0 or later.

Add this line to your application's Gemfile:

```ruby
gem 'ractor-lvar'
```

And then execute:

    $ bundle install

Or install it yourself as:

    $ gem install ractor-lvar

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake test-unit` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/ko1/ractor-lvar.


## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

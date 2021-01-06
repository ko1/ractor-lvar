# frozen_string_literal: true

require "test_helper"

class Ractor::LVarTest < Test::Unit::TestCase
  test "VERSION" do
    assert do
      ::Ractor::LVar.const_defined?(:VERSION)
    end
  end

  test 'Ractor::LVar can has a value' do
    lv = Ractor::LVar.new{ 42 }
    assert_equal 42, lv.ractor_local_value
  end

  test 'Ractor::LVar without initial value will return nil' do
    lv = Ractor::LVar.new
    assert_equal nil, lv.ractor_local_value
  end

  test 'Ractor::LVar can change the value' do
    lv = Ractor::LVar.new
    assert_equal nil, lv.ractor_local_value

    lv.ractor_local_value = :ok
    assert_equal :ok, lv.ractor_local_value
  end

  ## with Ractors
  N = 10_000
  test 'Ractor::LVar consistes with other Ractors' do
    lv = Ractor::LVar.new{ 0 }

    rets = 4.times.map{|i|
      Ractor.new lv, i do |lv, i|
        lv.ractor_local_value += i
        lv.ractor_local_value
      end.take
    }
    assert_equal [0, 1, 2, 3], rets
  end
end

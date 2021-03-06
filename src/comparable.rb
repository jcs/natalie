module Comparable
  def ==(other)
    (self <=> other) == 0
  end

  def !=(other)
    (self <=> other) != 0
  end

  def <(other)
    (self <=> other) < 0
  end

  def <=(other)
    (self <=> other) <= 0
  end

  def >(other)
    (self <=> other) > 0
  end

  def >=(other)
    (self <=> other) >= 0
  end
end

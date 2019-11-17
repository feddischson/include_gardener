require 'sidecar'

class MotorCycle_3
  def initialize(make, color)  
    # Instance variables  
    @make = make  
    @color = color
    @sidecar = Sidecar.new(1)
    end  
  def start_engine  
    if @engine_state  
      puts 'Engine is already Running'  
    else  
      @engine_state = true  
      puts 'Engine 3 Idle'  
    end  
  end  
end  

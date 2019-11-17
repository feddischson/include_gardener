class MotorCycle_2
  def initialize(make, color)  
    # Instance variables  
    @make = make  
    @color = color  
    end  
  def start_engine  
    if @engine_state  
      puts 'Engine is already Running'  
    else  
      @engine_state = true  
      puts 'Engine 2 Idle'  
    end  
  end  
end  

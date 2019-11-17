require 'motorcycle'  
require_relative 'motorcycle_2'
require './motorcycle_3'  
load './motorcycle_4.rb'

m1 = MotorCycle.new('Yamaha', 'red')  
m1.start_engine


m2 = MotorCycle_2.new('BMW', 'green')  
m2.start_engine

m = MotorCycle_3.new('Kawasaki', 'blue')  
m.start_engine

m = MotorCycle_4.new('Harley', 'black')  
m.start_engine


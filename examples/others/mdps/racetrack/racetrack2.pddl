(define (domain racetrack2)
  (:model (:dynamics :probabilistic) (:feedback :complete))
  (:types STATE)
  (:functions (x  STATE :integer[0,4])
              (y  STATE :integer[0,2])
              (dx STATE :integer[-5,5])
              (dy STATE :integer[-5,5]))
  (:predicates (valid STATE))
  (:external valid) 
  (:objects state lastState - STATE)

  (:ramification constraints
    :effect (:when (:not (valid state))
               (:set (x state) (x lastState))
               (:set (y state) (y lastState))
               (:set (dx state) 0)
               (:set (dy state) 0)))

  (:action control
    :parameters ?ax ?ay - :integer[-1,1]
    :effect
      (:probabilistic
         (0.9 (:set (x lastState) (x state))
              (:set (y lastState) (y state))	
              (:set (dx lastState) (dx state))	
              (:set (dy lastState) (dy state))	
              (:set (x state) (+ (x state) (+ (dx state) ?ax)))
              (:set (y state) (+ (y state) (+ (dy state) ?ay)))
              (:set (dx state) (+ (dx state) ?ax))
              (:set (dy state) (+ (dy state) ?ay)))
         (0.1 (:set (x lastState) (x state))
              (:set (y lastState) (y state))	
              (:set (dx lastState) (dx state))	
              (:set (dy lastState) (dy state))	
              (:set (x state) (+ (x state) (dx state)))
              (:set (y state) (+ (y state) (dy state))))))
)


(define (problem race)
  (:domain racetrack2)
  (:init (:set (dx state) 0)
         (:set (dy state) 0)
         (:set (x state) 0)
         (:set (y state) 0)
         (:set (dx lastState) 0)
         (:set (dy lastState) 0)
         (:set (x lastState) 0)
         (:set (y lastState) 0))
  (:goal (:and (= (x state) 4) (= (y state) 2)))
)


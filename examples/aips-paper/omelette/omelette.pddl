(define (domain omelette1)
  (:model (:dynamics :probabilistic) (:feedback :partial))
  (:types BOWL)
  (:functions (goods BOWL :integer[0,3])
              (bads BOWL :integer[0,3])
              (number BOWL :integer[0,3]))
  (:objects small large - BOWL good_egg holding - :boolean)

  (:ramification ram1 
    :parameters ?bowl - BOWL
    :effect (:set (number ?bowl) (+ (goods ?bowl) (bads ?bowl))))
               
  (:action grab
    :precondition (= holding false)
    :effect (:probabilistic
                 (0.25 (:set good_egg true)
                       (:set holding true))
                 (0.75 (:set good_egg false)
                       (:set holding true))))

  (:action break_egg
    :parameters ?bowl - BOWL
    :precondition (:and (< (number ?bowl) 3)
                        (= holding true))
    :effect (:when (= good_egg true)  (:set holding false)
                                      (:set good_egg false)  ; just to generate less belief states
                                      (:set (goods ?bowl) (+ (goods ?bowl) 1)))
            (:when (= good_egg false) (:set holding false)
                                      (:set (bads ?bowl) (+ (bads ?bowl) 1))))

  (:action pour
    :parameters ?bowl1 ?bowl2 - BOWL
    :precondition (:and (:not (= ?bowl1 ?bowl2))
                        (= holding false)
                        (<= (+ (number ?bowl1) (number ?bowl2)) 3))
    :effect (:set (goods ?bowl2) (+ (goods ?bowl2) (goods ?bowl1)))
            (:set (bads ?bowl2) (+ (bads ?bowl2) (bads ?bowl1)))
            (:set (goods ?bowl1) 0)
            (:set (bads ?bowl1) 0))
   
  (:action clean
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :effect (:set (goods ?bowl) 0)
            (:set (bads ?bowl) 0))

  (:action inspect
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :observation (= (bads ?bowl) 0)))


(define (problem p1)
  (:domain omelette1)
  (:init (:set (goods small) 0)
         (:set (bads small) 0)
         (:set (goods large) 0)
         (:set (bads large) 0)
	 (:set holding false)
	 (:set good_egg false))
  (:goal (:and (= (goods large) 3)
               (= (bads large) 0))))








(define (domain omelette2)
  (:model (:dynamics :probabilistic) (:feedback :partial))
   (:types BOWL)
  (:functions (goods BOWL :integer[0,3])
              (bads BOWL :integer[0,3])
              (number BOWL :integer[0,3]))
  (:objects small large - BOWL good_egg holding - :boolean)

  (:ramification ram1 
    :parameters ?bowl - BOWL
    :effect (:set (number ?bowl) (+ (goods ?bowl) (bads ?bowl))))
               
  (:action grab
    :precondition (= holding false)
    :effect (:probabilistic
                 (0.5 (:set good_egg true)
                      (:set holding true))
                 (0.5 (:set good_egg false)
                      (:set holding true))))

  (:action break_egg
    :parameters ?bowl - BOWL
    :precondition (:and (< (number ?bowl) 3)
                        (= holding true))
    :effect (:when (= good_egg true)  (:set holding false)
                                      (:set good_egg false)  ; just to generate less belief states
                                      (:set (goods ?bowl) (+ (goods ?bowl) 1)))
            (:when (= good_egg false) (:set holding false)
                                      (:set (bads ?bowl) (+ (bads ?bowl) 1))))

  (:action pour
    :parameters ?bowl1 ?bowl2 - BOWL
    :precondition (:and (:not (= ?bowl1 ?bowl2))
                        (= holding false)
                        (<= (+ (number ?bowl1) (number ?bowl2)) 3))
    :effect (:set (goods ?bowl2) (+ (goods ?bowl2) (goods ?bowl1)))
            (:set (bads ?bowl2) (+ (bads ?bowl2) (bads ?bowl1)))
            (:set (goods ?bowl1) 0)
            (:set (bads ?bowl1) 0))
   
  (:action clean
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :effect (:set (goods ?bowl) 0)
            (:set (bads ?bowl) 0))

  (:action inspect
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :observation (= (bads ?bowl) 0)))


(define (problem p2)
  (:domain omelette2)
  (:init (:set (goods small) 0)
         (:set (bads small) 0)
         (:set (goods large) 0)
         (:set (bads large) 0)
	 (:set holding false)
	 (:set good_egg false))
  (:goal (:and (= (goods large) 3)
               (= (bads large) 0))))




(define (domain omelette3)
  (:model (:dynamics :probabilistic) (:feedback :partial))
   (:types BOWL)
  (:functions (goods BOWL :integer[0,3])
              (bads BOWL :integer[0,3])
              (number BOWL :integer[0,3]))
  (:objects small large - BOWL good_egg holding - :boolean)

  (:ramification ram1 
    :parameters ?bowl - BOWL
    :effect (:set (number ?bowl) (+ (goods ?bowl) (bads ?bowl))))
               
  (:action grab
    :precondition (= holding false)
    :effect (:probabilistic
                 (0.75 (:set good_egg true)
                       (:set holding true))
                 (0.25 (:set good_egg false)
                       (:set holding true))))

  (:action break_egg
    :parameters ?bowl - BOWL
    :precondition (:and (< (number ?bowl) 3)
                        (= holding true))
    :effect (:when (= good_egg true)  (:set holding false)
                                      (:set good_egg false)  ; just to generate less belief states
                                      (:set (goods ?bowl) (+ (goods ?bowl) 1)))
            (:when (= good_egg false) (:set holding false)
                                      (:set (bads ?bowl) (+ (bads ?bowl) 1))))

  (:action pour
    :parameters ?bowl1 ?bowl2 - BOWL
    :precondition (:and (:not (= ?bowl1 ?bowl2))
                        (= holding false)
                        (<= (+ (number ?bowl1) (number ?bowl2)) 3))
    :effect (:set (goods ?bowl2) (+ (goods ?bowl2) (goods ?bowl1)))
            (:set (bads ?bowl2) (+ (bads ?bowl2) (bads ?bowl1)))
            (:set (goods ?bowl1) 0)
            (:set (bads ?bowl1) 0))
   
  (:action clean
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :effect (:set (goods ?bowl) 0)
            (:set (bads ?bowl) 0))

  (:action inspect
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :observation (= (bads ?bowl) 0)))


(define (problem p3)
  (:domain omelette3)
  (:init (:set (goods small) 0)
         (:set (bads small) 0)
         (:set (goods large) 0)
         (:set (bads large) 0)
	 (:set holding false)
	 (:set good_egg false))
  (:goal (:and (= (goods large) 3) (= (bads large) 0))))



(define (domain omelette4)
  (:model (:dynamics :probabilistic) (:feedback :partial))
   (:types BOWL)
  (:functions (goods BOWL :integer[0,3])
              (bads BOWL :integer[0,3])
              (number BOWL :integer[0,3]))
  (:objects small large - BOWL good_egg holding - :boolean)

  (:ramification ram1 
    :parameters ?bowl - BOWL
    :effect (:set (number ?bowl) (+ (goods ?bowl) (bads ?bowl))))
               
  (:action grab
    :precondition (= holding false)
    :effect (:probabilistic
                 (0.90 (:set good_egg true)
                       (:set holding true))
                 (0.10 (:set good_egg false)
                       (:set holding true))))

  (:action break_egg
    :parameters ?bowl - BOWL
    :precondition (:and (< (number ?bowl) 3)
                        (= holding true))
    :effect (:when (= good_egg true)  (:set holding false)
                                      (:set good_egg false)  ; just to generate less belief states
                                      (:set (goods ?bowl) (+ (goods ?bowl) 1)))
            (:when (= good_egg false) (:set holding false)
                                      (:set (bads ?bowl) (+ (bads ?bowl) 1))))

  (:action pour
    :parameters ?bowl1 ?bowl2 - BOWL
    :precondition (:and (:not (= ?bowl1 ?bowl2))
                        (= holding false)
                        (<= (+ (number ?bowl1) (number ?bowl2)) 3))
    :effect (:set (goods ?bowl2) (+ (goods ?bowl2) (goods ?bowl1)))
            (:set (bads ?bowl2) (+ (bads ?bowl2) (bads ?bowl1)))
            (:set (goods ?bowl1) 0)
            (:set (bads ?bowl1) 0))
   
  (:action clean
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :effect (:set (goods ?bowl) 0)
            (:set (bads ?bowl) 0))

  (:action inspect
    :parameters ?bowl - BOWL
    :precondition (= holding false)
    :observation (= (bads ?bowl) 0)))


(define (problem p4)
  (:domain omelette4)
  (:init (:set (goods small) 0)
         (:set (bads small) 0)
         (:set (goods large) 0)
         (:set (bads large) 0)
	 (:set holding false)
	 (:set good_egg false))
  (:goal (:and (= (goods large) 3)
               (= (bads large) 0))))





;;
;; Sylvie Thiebaux's PSR domain
;;
;; This encoding corresponds to the original description of PSR in the ECAI-02 paper.

(define (domain std_worst)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:types SWITCH SIDE LINE MODE)
  (:functions
    (ac_mode SWITCH MODE)   ; actuator mode; can be ok, out-of-order, or liar
    (fd_mode SWITCH MODE)   ; fault detector mode; can be ok, out-of-order, liar
    (opposite SIDE SIDE)    ; opposite side of a side
  )
  (:predicates 
    (ext LINE SWITCH SIDE)  ; ext(l,x,sx): line l is connected to side sx of x
    (breaker SWITCH)        ; breaker(x): switch x is a circuit-breaker
    (closed SWITCH)         ; closed(x): switch x is closed
    (faulty LINE)           ; faulty(l): line l is faulty
    (pd_ok SWITCH)          ; pd_ok(x): true iff the position detector for x is ok
  )
  (:objects need_wait done - :boolean
            side1 side2 - SIDE 
            ok out liar - MODE
  )

  ; PREDICATES

  ; (con ?x ?sx ?y ?sy) = true iff side ?sx of ?x is connected to side ?sy of ?y
  (:predicate (con ?x - SWITCH ?sx - SIDE ?y - SWITCH ?sy - SIDE)
    (:and (:if (= ?x ?y) (:not (= ?sx ?sy)))
          (:exists ?l - LINE
            (:and (ext ?l ?x ?sx) (ext ?l ?y ?sy))))
  )

  ; (upstream ?x ?sx ?y ?sy) = true iff side ?sy of ?y is downstream from side 
  ; ?sx of ?x through a fed path. We consider 4 cases to control the computation:
  ;         sx x         sy y
  ;   a)       B------------O
  ;
  ;         sx x         sz z         sy y
  ;   b)       B--- .... ---K------------O
  ;
  ;         sz z         sx x         sy y
  ;   c)       B--- .... ---K------------O
  ;
  ;                      sx x         su u         sy y
  ;   d)       B--- .... ---K--- .... ---K------------O
  ; where B is a closed breaker and K is a closed switch
  (:predicate (upstream ?x - SWITCH ?sx - SIDE ?y - SWITCH ?sy - SIDE)
    (:and (closed ?x)
          (:or (:and (breaker ?x) (con ?x (opposite ?sx) ?y ?sy))
               (:and (breaker ?x)
                     (:exists ?z - SWITCH
                       (:and (closed ?z)
                             (:exists ?sz - SIDE
                               (:and (con ?z (opposite ?sz) ?y ?sy)
                                     (upstream ?x ?sx ?z ?sz))))))
               (:and (:not (breaker ?x))
                     (:or (:and (con ?x (opposite ?sx) ?y ?sy)
                                (:exists ?z - SWITCH
                                  (:and (breaker ?z)
                                        (closed ?z)
                                        (:exists ?sz - SIDE
                                          (upstream ?z ?sz ?x ?sx)))))
                          (:exists ?u - SWITCH
                            (:and (closed ?u)
                                  (:exists ?su - SIDE
                                    (:and (con ?u (opposite ?su) ?y ?sy)
                                          (upstream ?x ?sx ?u ?su)))))))))
  )

  ; (fed ?x) = true iff there is a breaker that feds ?x:
  ;            x       
  ;   a)       B
  ;
  ;         sy y         sx x
  ;   b)       B--- .... ---K
  (:predicate (fed ?x - SWITCH)
    (:or (:and (breaker ?x) (closed ?x))
	 (:exists ?y - SWITCH
           (:and (breaker ?y)
                 (:exists ?sy - SIDE
                   (:exists ?sx - SIDE (upstream ?y ?sy ?x ?sx))))))
  )

  ; (affected ?x) = true iff there is a fault downstream ?x and ?x is fed:
  ;         sy y         sx x
  ;   a)       B--- .... ---K------- ...
  ;                             ^
  ;                      sx x         sy y
  ;   b)       B--- .... ---K--- .... ---K------- ...
  (:predicate (affected ?x - SWITCH)
    (:and (closed ?x)
          (:exists ?l - LINE
            (:and (faulty ?l)
                  (:or (:and (breaker ?x)
                             (:exists ?sx - SIDE (ext ?l ?x ?sx)))
                       (:exists ?sx - SIDE
                         (:and (ext ?l ?x (opposite ?sx))
                               (:exists ?y - SWITCH
                                 (:and (breaker ?y)
                                       (closed ?y)
                                       (:exists ?sy - SIDE
                                         (upstream ?y ?sy ?x ?sx)))))
                       )
                       (:exists ?y - SWITCH
                         (:exists ?sy - SIDE
                           (:and (ext ?l ?y (opposite ?sy))
                                 (closed ?y)
                                 (:exists ?sx - SIDE
                                   (upstream ?x ?sx ?y ?sy)))))))))
  )

  ; (closed_path ?x ?sx ?l) = true iff there is a path made of non-faulty 
  ; lines and *closed* switches downstream from ?x to ?l
  (:predicate (closed_path ?x - SWITCH ?sx - SIDE ?l - LINE)
    (:and (closed ?x)
          (:not (faulty ?l))
          (:or (ext ?l ?x (opposite ?sx))
               (:exists ?l2 - LINE
                 (:and (:not (faulty ?l2))
                       (ext ?l2 ?x (opposite ?sx))
                       (:exists ?z - SWITCH
                         (:and (:not (= ?x ?z))
                               (closed ?z)
                               (:exists ?sz - SIDE
                                 (:and (ext ?l2 ?z ?sz)
                                       (closed_path ?z ?sz ?l)))))))))
  )

  ; (fed_line ?l) = true iff there is a breaker ?x and side ?sx such that
  ; (closed_path ?x ?sx ?l)
  (:predicate (fed_line ?l - LINE)
    (:exists ?x - SWITCH
      (:and (breaker ?x)
            (closed ?x)
            (:exists ?sx - SIDE (closed_path ?x ?sx ?l))))
  )

  ; (bad ?x) = true iff after closing ?x there is a loop or double fed switch.
  ; Assume network was good before so any loop or double fed would involve ?x
  ; in case ?x is not breaker, or it will be a loop if ?x is breaker.
  (:predicate (bad ?x - SWITCH)
    (:and (:if (:not (breaker ?x))
               (:exists ?y - SWITCH
                 (:and (breaker ?y)
                       (:exists ?sy - SIDE (upstream ?y ?sy ?x side1))
                         (:exists ?z - SWITCH
                           (:and (breaker ?z)
                                 (:exists ?sz - SIDE 
                                   (upstream ?z ?sz ?x side2)))))))
          (:if (breaker ?x)
               (:exists ?sx - SIDE
                 (:exists ?l - LINE
                   (:and (ext ?l ?x ?sx)
                         (:exists ?y - SWITCH
                           (:exists ?sy - SIDE
                             (:and (ext ?l ?y ?sy)
                                   (closed_path ?y ?sy ?l)))))))))
  )

  ; RAMIFICATIONS

  ; open breakers if fault downstream
  (:ramification open_ramification
    :parameters ?x - SWITCH
    :effect (:when (:and (= need_wait false) (breaker ?x) (affected ?x))
                   (:set (closed ?x) false))
  )

  ; ACTIONS

  (:action wait
    :precondition (= need_wait true)
    :effect       (:set need_wait false)
    :cost         0
  )

  (:action finish
    :precondition (= need_wait false)
    :effect (:set done true)
    :cost (:sum ?l - LINE (:if (:or (faulty ?l) (fed_line ?l)) 0 5))
  )

  (:action close
    :parameters   ?x - SWITCH
    :precondition (:and (= need_wait false) (:not (bad ?x)))
    :effect       (:set need_wait true)
                  (:when (= (ac_mode ?x) ok) (:set (closed ?x) true))
    :observation
      (= (ac_mode ?x) out)
      (:vector ?y - SWITCH (:if (pd_ok ?y) (:formula (closed ?y)) false))
      (:vector ?y - SWITCH
         (:if (:and (:not (= (fd_mode ?y) out)) (fed ?y))
              (:if (= (fd_mode ?y) ok)
                   (:formula (affected ?y))
                   (:formula (:not (affected ?y))))
              false)
      )
  )

  (:action open
    :parameters   ?x - SWITCH
    :precondition (= need_wait false)
    :effect       (:set need_wait true)
                  (:when (= (ac_mode ?x) ok) (:set (closed ?x) false))
    :observation
      (= (ac_mode ?x) out)
      (:vector ?y - SWITCH (:if (pd_ok ?y) (:formula (closed ?y)) false))
      (:vector ?y - SWITCH
         (:if (:and (:not (= (fd_mode ?y) out)) (fed ?y))
              (:if (= (fd_mode ?y) ok)
                   (:formula (affected ?y))
                   (:formula (:not (affected ?y))))
              false)
      )
  )
)

;;
;; Average Case
;;

(define (domain std_avg)
  (:model (:dynamics :deterministic) (:feedback :partial))
  (:types SWITCH SIDE LINE MODE)
  (:functions
    (ac_mode SWITCH MODE)   ; actuator mode; can be ok, out-of-order, or liar
    (fd_mode SWITCH MODE)   ; fault detector mode; can be ok, out-of-order, liar
    (opposite SIDE SIDE)    ; opposite side of a side
  )
  (:predicates 
    (ext LINE SWITCH SIDE)  ; ext(l,x,sx): line l is connected to side sx of x
    (breaker SWITCH)        ; breaker(x): switch x is a circuit-breaker
    (closed SWITCH)         ; closed(x): switch x is closed
    (faulty LINE)           ; faulty(l): line l is faulty
    (pd_ok SWITCH)          ; pd_ok(x): true iff the position detector for x is ok
  )
  (:objects need_wait done - :boolean
            side1 side2 - SIDE 
            ok out liar - MODE
  )

  ; PREDICATES

  ; (con ?x ?sx ?y ?sy) = true iff side ?sx of ?x is connected to side ?sy of ?y
  (:predicate (con ?x - SWITCH ?sx - SIDE ?y - SWITCH ?sy - SIDE)
    (:and (:if (= ?x ?y) (:not (= ?sx ?sy)))
          (:exists ?l - LINE
            (:and (ext ?l ?x ?sx) (ext ?l ?y ?sy))))
  )

  ; (upstream ?x ?sx ?y ?sy) = true iff side ?sy of ?y is downstream from side 
  ; ?sx of ?x through a fed path. We consider 4 cases to control the computation:
  ;         sx x         sy y
  ;   a)       B------------O
  ;
  ;         sx x         sz z         sy y
  ;   b)       B--- .... ---K------------O
  ;
  ;         sz z         sx x         sy y
  ;   c)       B--- .... ---K------------O
  ;
  ;                      sx x         su u         sy y
  ;   d)       B--- .... ---K--- .... ---K------------O
  ; where B is a closed breaker and K is a closed switch
  (:predicate (upstream ?x - SWITCH ?sx - SIDE ?y - SWITCH ?sy - SIDE)
    (:and (closed ?x)
          (:or (:and (breaker ?x) (con ?x (opposite ?sx) ?y ?sy))
               (:and (breaker ?x)
                     (:exists ?z - SWITCH
                       (:and (closed ?z)
                             (:exists ?sz - SIDE
                               (:and (con ?z (opposite ?sz) ?y ?sy)
                                     (upstream ?x ?sx ?z ?sz))))))
               (:and (:not (breaker ?x))
                     (:or (:and (con ?x (opposite ?sx) ?y ?sy)
                                (:exists ?z - SWITCH
                                  (:and (breaker ?z)
                                        (closed ?z)
                                        (:exists ?sz - SIDE
                                          (upstream ?z ?sz ?x ?sx)))))
                          (:exists ?u - SWITCH
                            (:and (closed ?u)
                                  (:exists ?su - SIDE
                                    (:and (con ?u (opposite ?su) ?y ?sy)
                                          (upstream ?x ?sx ?u ?su)))))))))
  )

  ; (fed ?x) = true iff there is a breaker that feds ?x:
  ;            x       
  ;   a)       B
  ;
  ;         sy y         sx x
  ;   b)       B--- .... ---K
  (:predicate (fed ?x - SWITCH)
    (:or (:and (breaker ?x) (closed ?x))
	 (:exists ?y - SWITCH
           (:and (breaker ?y)
                 (:exists ?sy - SIDE
                   (:exists ?sx - SIDE (upstream ?y ?sy ?x ?sx))))))
  )

  ; (affected ?x) = true iff there is a fault downstream ?x and ?x is fed:
  ;         sy y         sx x
  ;   a)       B--- .... ---K------- ...
  ;                             ^
  ;                      sx x         sy y
  ;   b)       B--- .... ---K--- .... ---K------- ...
  (:predicate (affected ?x - SWITCH)
    (:and (closed ?x)
          (:exists ?l - LINE
            (:and (faulty ?l)
                  (:or (:and (breaker ?x)
                             (:exists ?sx - SIDE (ext ?l ?x ?sx)))
                       (:exists ?sx - SIDE
                         (:and (ext ?l ?x (opposite ?sx))
                               (:exists ?y - SWITCH
                                 (:and (breaker ?y)
                                       (closed ?y)
                                       (:exists ?sy - SIDE
                                         (upstream ?y ?sy ?x ?sx)))))
                       )
                       (:exists ?y - SWITCH
                         (:exists ?sy - SIDE
                           (:and (ext ?l ?y (opposite ?sy))
                                 (closed ?y)
                                 (:exists ?sx - SIDE
                                   (upstream ?x ?sx ?y ?sy)))))))))
  )

  ; (closed_path ?x ?sx ?l) = true iff there is a path made of non-faulty 
  ; lines and *closed* switches downstream from ?x to ?l
  (:predicate (closed_path ?x - SWITCH ?sx - SIDE ?l - LINE)
    (:and (closed ?x)
          (:not (faulty ?l))
          (:or (ext ?l ?x (opposite ?sx))
               (:exists ?l2 - LINE
                 (:and (:not (faulty ?l2))
                       (ext ?l2 ?x (opposite ?sx))
                       (:exists ?z - SWITCH
                         (:and (:not (= ?x ?z))
                               (closed ?z)
                               (:exists ?sz - SIDE
                                 (:and (ext ?l2 ?z ?sz)
                                       (closed_path ?z ?sz ?l)))))))))
  )

  ; (fed_line ?l) = true iff there is a breaker ?x and side ?sx such that
  ; (closed_path ?x ?sx ?l)
  (:predicate (fed_line ?l - LINE)
    (:exists ?x - SWITCH
      (:and (breaker ?x)
            (closed ?x)
            (:exists ?sx - SIDE (closed_path ?x ?sx ?l))))
  )

  ; (bad ?x) = true iff after closing ?x there is a loop or double fed switch.
  ; Assume network was good before so any loop or double fed would involve ?x
  ; in case ?x is not breaker, or it will be a loop if ?x is breaker.
  (:predicate (bad ?x - SWITCH)
    (:and (:if (:not (breaker ?x))
               (:exists ?y - SWITCH
                 (:and (breaker ?y)
                       (:exists ?sy - SIDE (upstream ?y ?sy ?x side1))
                         (:exists ?z - SWITCH
                           (:and (breaker ?z)
                                 (:exists ?sz - SIDE 
                                   (upstream ?z ?sz ?x side2)))))))
          (:if (breaker ?x)
               (:exists ?sx - SIDE
                 (:exists ?l - LINE
                   (:and (ext ?l ?x ?sx)
                         (:exists ?y - SWITCH
                           (:exists ?sy - SIDE
                             (:and (ext ?l ?y ?sy)
                                   (closed_path ?y ?sy ?l)))))))))
  )

  ; RAMIFICATIONS

  ; open breakers if fault downstream
  (:ramification open_ramification
    :parameters ?x - SWITCH
    :effect (:when (:and (= need_wait false) (breaker ?x) (affected ?x))
                   (:set (closed ?x) false))
  )

  ; ACTIONS

  (:action wait
    :precondition (= need_wait true)
    :effect       (:set need_wait false)
    :cost         0
  )

  (:action finish
    :precondition (= need_wait false)
    :effect (:set done true)
    :cost (:sum ?l - LINE (:if (:or (faulty ?l) (fed_line ?l)) 0 5))
  )

  (:action close
    :parameters   ?x - SWITCH
    :precondition (:and (= need_wait false) (:not (bad ?x)))
    :effect       (:set need_wait true)
                  (:when (= (ac_mode ?x) ok) (:set (closed ?x) true))
    :observation
      (= (ac_mode ?x) out)
      (:vector ?y - SWITCH (:if (pd_ok ?y) (:formula (closed ?y)) false))
      (:vector ?y - SWITCH
         (:if (:and (:not (= (fd_mode ?y) out)) (fed ?y))
              (:if (= (fd_mode ?y) ok)
                   (:formula (affected ?y))
                   (:formula (:not (affected ?y))))
              false)
      )
  )

  (:action open
    :parameters   ?x - SWITCH
    :precondition (= need_wait false)
    :effect       (:set need_wait true)
                  (:when (= (ac_mode ?x) ok) (:set (closed ?x) false))
    :observation
      (= (ac_mode ?x) out)
      (:vector ?y - SWITCH (:if (pd_ok ?y) (:formula (closed ?y)) false))
      (:vector ?y - SWITCH
         (:if (:and (:not (= (fd_mode ?y) out)) (fed ?y))
              (:if (= (fd_mode ?y) ok)
                   (:formula (affected ?y))
                   (:formula (:not (affected ?y))))
              false)
      )
  )
)

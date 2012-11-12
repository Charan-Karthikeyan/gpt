;; Wumpus World [Russell book, p. 153]
;; ===================================
;;
;; small grid world:
;;
;;    (0,2)  (1,2)  (2,2)
;;    (0,1)  (1,1)  (2,1)
;;    (0,0)  (1,0)  (2,0)
;;
;; Variables:
;; ==========
;; 
;;   facing = { 0=north, 1=east, 2=south, 3=west }
;;   arrows = number of remaining arrows
;;   num_wumpus = number of remaining wumpusses (?)
;;   x, y = grid position 
;;   ok = true if we're still alive
;;   incave = true if we're still in the frigging cave
;;

(define (domain wumpus)
  ;(:model (:dynamics :non-deterministic) (:feedback :partial))
  (:model (:dynamics :probabilistic) (:feedback :partial))
  (:types PIT WUMPUS GOLD)
  (:functions (pxpos PIT :integer[0,2])
              (pypos PIT :integer[0,2])
              (wxpos WUMPUS :integer[0,2])
              (wypos WUMPUS :integer[0,2])
              (gxpos GOLD :integer[0,2])
              (gypos GOLD :integer[0,2]))
  (:predicates (taken GOLD) (alive WUMPUS))
  (:objects arrows - :integer[0,10]
            num_wumpus - :integer[0,10]
            facing - :integer[0,3]
            x y - :integer[0,2]
            ok - :boolean
            incave - :boolean)

  ;; useful predicates
  (:predicate (glitter ?x ?y - :integer[0,2])
     (:exists ?g - GOLD (:and (:not (taken ?g)) (= (gxpos ?g) ?x) (= (gypos ?g) ?y)))
  )

  (:predicate (breeze ?x ?y - :integer[0,2])
     (:exists ?p - PIT
        (:or (:and (= (pxpos ?p) ?x) (= (pypos ?p) (+ ?y 1)))
             (:and (= (pxpos ?p) ?x) (= (pypos ?p) (- ?y 1)))
             (:and (= (pypos ?p) ?y) (= (pxpos ?p) (+ ?x 1)))
             (:and (= (pypos ?p) ?y) (= (pxpos ?p) (- ?x 1))))
     )
  )

  (:predicate (stench ?x ?y - :integer[0,2])
     (:exists ?w - WUMPUS
        (:or (:and (= (wxpos ?w) ?x) (= (wypos ?w) (+ ?y 1)))
             (:and (= (wxpos ?w) ?x) (= (wypos ?w) (- ?y 1)))
             (:and (= (wypos ?w) ?y) (= (wxpos ?w) (+ ?x 1)))
             (:and (= (wypos ?w) ?y) (= (wxpos ?w) (- ?x 1))))
     )
  )

  ;; shoot an arrow in straight direction killing at most one wumpus in its path
  (:action shoot
     :precondition (:and (= incave true) (= ok true) (> arrows 0))
     :effect
       (:set arrows (- arrows 1))
       (:foreach ?w - WUMPUS
          (:when (:and (= num_wumpus *num_wumpus) ;; while haven't killed
                       (:or (:and (= facing 0) (= x (wxpos ?w)) (< y (wypos ?w)))
                            (:and (= facing 2) (= x (wxpos ?w)) (> y (wypos ?w)))
                            (:and (= facing 1) (= y (wypos ?w)) (< x (wxpos ?w)))
                            (:and (= facing 3) (= y (wypos ?w)) (> x (wxpos ?w)))
                       ))
                 (:set (alive ?w) false)
                 (:set num_wumpus (- num_wumpus 1))
          )
       )
     :observation num_wumpus
  )

  ;; move one step in facing direction
  (:action move
     :precondition (:and (= incave true) (= ok true))

     :effect 
       (:when (= facing 0) (:set y (:if (= y 2) 2 (+ y 1))))
       (:when (= facing 2) (:set y (:if (= y 0) 0 (- y 1))))
       (:when (= facing 1) (:set x (:if (= x 2) 2 (+ x 1))))
       (:when (= facing 3) (:set x (:if (= x 0) 0 (- x 1))))

       ;; dead conditions
       (:when (:exists ?w - WUMPUS (:and (= *x (wxpos ?w)) (= *y (wypos ?w))))
              (:set ok false))
       (:when (:exists ?p - PIT (:and (= *x (pxpos ?p)) (= *y (pypos ?p))))
              (:set ok false))

     :observation ok (glitter x y) (breeze x y) (stench x y)
  )

  (:action rot_right
     :precondition (:and (= incave true) (= ok true))
     :effect (:set facing (+<4> facing 1))
     :observation ok (glitter x y) (breeze x y) (stench x y)
  )

  (:action rot_left
     :precondition (:and (= incave true) (= ok true))
     :effect (:set facing (-<4> facing 1))
     :observation ok (glitter x y) (breeze x y) (stench x y)
  )

  (:action grab
     :precondition (:and (= incave true) (= ok true))
     :effect 
       (:foreach ?g - GOLD
          (:when (:and (= x (gxpos ?g)) (= y (gypos ?g)) (:not (taken ?g)))
                 (:set (taken ?g) true))
       )
  )

  ;; the following actions reset all variables in order to reduce state-space
  (:action climb
     :precondition (:and (= incave true) (= ok true) (= x 0) (= y 0))
     :effect (:set incave false)
             (:set facing 0)
             (:foreach ?g - GOLD
                (:set (gxpos ?g) 0)
                (:set (gypos ?g) 0))
             (:foreach ?p - PIT
                (:set (pxpos ?p) 0)
                (:set (pypos ?p) 0))
             (:foreach ?w - WUMPUS
                (:set (wxpos ?w) 0)
                (:set (wypos ?w) 0)
                (:set (alive ?w) false))
     :cost (:sum ?g - GOLD (:if (:not (taken ?g)) 100 0))
  )

  ;; the finish action is used only when we are dead
  (:action finish
     :precondition (:and (= incave true) (= ok false))
     :effect (:set incave false)
             (:set facing 0)
             (:set ok true)
             (:set x 0)
             (:set y 0)
             (:foreach ?g - GOLD
                (:set (gxpos ?g) 0)
                (:set (gypos ?g) 0))
             (:foreach ?p - PIT
                (:set (pxpos ?p) 0)
                (:set (pypos ?p) 0))
             (:foreach ?w - WUMPUS
                (:set (wxpos ?w) 0)
                (:set (wypos ?w) 0)
                (:set (alive ?w) false))
     :cost 1000
  )
)

;; PROBLEM: 0 gold, 0 wumpus, 1 pit, 0 arrows
(define (problem p0)
  (:domain wumpus)
  (:objects p1 - PIT)
  (:init (:set x 0)
         (:set y 0)
         (:set facing 0)
         (:set ok true)
         (:set incave true)
         (:set arrows 0)
         (:set num_wumpus 0)

         ;; set pit
         (:set (pxpos p1) :in { 0 1 2 })
         (:set (pypos p1) :in { 0 1 2 })
         (:not (:and (= (pxpos p1) 0) (= (pypos p1) 0)))
  )
  (:goal (= incave false))
)

;; PROBLEM: 1 gold, 0 wumpus, 1 pit, 0 arrows
(define (problem p1)
  (:domain wumpus)
  (:objects g1 - GOLD p1 - PIT)
  (:init (:set x 0)
         (:set y 0)
         (:set facing 0)
         (:set ok true)
         (:set incave true)
         (:set arrows 0)
         (:set num_wumpus 0)

         ;; set gold
         (:set (gxpos g1) :in { 0 1 2 })
         (:set (gypos g1) :in { 0 1 2 })
         (:set (taken g1) false)
         (:not (:and (= (gxpos g1) 0) (= (gypos g1) 0)))

         ;; set pit
         (:set (pxpos p1) :in { 0 1 2 })
         (:set (pypos p1) :in { 0 1 2 })
         (:not (:and (= (pxpos p1) 0) (= (pypos p1) 0)))
         (:not (:and (= (pxpos p1) (gxpos g1)) (= (pypos p1) (gypos g1))))
  )
  (:goal (= incave false))
)

;; PROBLEM: 1 gold, 1 wumpus, 0 pit, 1 arrows
(define (problem p2)
  (:domain wumpus)
  (:objects g1 - GOLD w1 - WUMPUS)
  (:init (:set x 0)
         (:set y 0)
         (:set facing 0)
         (:set ok true)
         (:set incave true)
         (:set arrows 1)
         (:set num_wumpus 1)

         ;; set gold
         (:set (gxpos g1) :in { 0 1 2 })
         (:set (gypos g1) :in { 0 1 2 })
         (:set (taken g1) false)
         (:not (:and (= (gxpos g1) 0) (= (gypos g1) 0)))

         ;; set wumpus
         (:set (wxpos w1) :in { 0 1 2 })
         (:set (wypos w1) :in { 0 1 2 })
         (:not (:and (= (wxpos w1) 0) (= (wypos w1) 0)))
         (:not (:and (= (wxpos w1) (gxpos g1)) (= (wypos w1) (gypos g1))))
  )
  (:goal (= incave false))
)


;; PROBLEM: 1 gold, 1 wumpus, 1 pit, 1 arrows
(define (problem p3)
  (:domain wumpus)
  (:objects g1 - GOLD w1 - WUMPUS p1 - PIT)
  (:init (:set x 0)
         (:set y 0)
         (:set facing 0)
         (:set ok true)
         (:set incave true)
         (:set arrows 1)
         (:set num_wumpus 1)

         ;; set gold
         (:set (gxpos g1) :in { 0 1 2 })
         (:set (gypos g1) :in { 0 1 2 })
         (:set (taken g1) false)
         (:not (:and (= (gxpos g1) 0) (= (gypos g1) 0)))

         ;; set wumpus
         (:set (wxpos w1) :in { 0 1 2 })
         (:set (wypos w1) :in { 0 1 2 })
         (:not (:and (= (wxpos w1) 0) (= (wypos w1) 0)))
         (:not (:and (= (wxpos w1) (gxpos g1)) (= (wypos w1) (gypos g1))))

         ;; set pit
         (:set (pxpos p1) :in { 0 1 2 })
         (:set (pypos p1) :in { 0 1 2 })
         (:not (:and (= (pxpos p1) 0) (= (pypos p1) 0)))
         (:not (:and (= (pxpos p1) (gxpos g1)) (= (pypos p1) (gypos g1))))
         (:not (:and (= (pxpos p1) (wxpos w1)) (= (pypos p1) (wypos w1))))
  )
  (:goal (= incave false))
)

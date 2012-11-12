(define (domain blocks)
  (:model (:dynamics :deterministic) (:feedback :null))
  (:types BLOCK)
  (:functions (on BLOCK BLOCK))
  (:objects table - BLOCK)

  (:predicate (above ?x ?y - BLOCK)
    (:or (= (on ?x) ?y)
         (:exists ?z - BLOCK 
           (:and (= (on ?z) ?y) (above ?x ?z)))))

  (:predicate (clear ?x - BLOCK)
    (:or (= ?x table)
         (:forall ?z - BLOCK (:not (= (on ?z) ?x)))))

  (:action move
    :parameters ?x ?y - BLOCK
    :precondition (:and (clear ?x) (clear ?y)
                        (:not (= ?x table)) (:not (= ?x ?y)))
    :effect (:set (on ?x) ?y)
  )

  (:action knock   ; Knocks down to the table all objects above x
    :parameters ?x - BLOCK
    :effect (:forall ?y - BLOCK
               (:when (above ?y ?x) (:set (on ?y) table)))
  )
)

(define (problem p1)
  (:domain blocks)
  (:objects a b c - BLOCK)
  (:init (:set (on a) :in { b c table })
         (:set (on b) :in { a c table })
         (:set (on c) :in { a b table })
	 (:forall ?x - BLOCK       ; no cycles
		  (:if (:not (= ?x table)) (:not (above ?x ?x))))
	 (:forall ?x - BLOCK       ; no two blocks on same
		  (:forall ?y - BLOCK
			   (:if (= (on ?x) (on ?y))
				(:or (= ?x ?y) (= (on ?x) table)))))
  )
  (:goal (:and (= (on a) b) (= (on b) c)))
)

(define (problem p2)
  (:domain blocks)
  (:objects a b c d - BLOCK)
  (:init (:set (on a) :in { b c d table })
         (:set (on b) :in { a c d table })
         (:set (on c) :in { a b d table })
         (:set (on d) :in { a b c table })
	 (:forall ?x - BLOCK       ; no cycles
		  (:if (:not (= ?x table)) (:not (above ?x ?x))))
	 (:forall ?x - BLOCK       ; no two blocks on same
		  (:forall ?y - BLOCK
			   (:if (= (on ?x) (on ?y))
				(:or (= ?x ?y) (= (on ?x) table)))))
  )
  (:goal (:and (= (on a) b) (= (on b) c)))
)

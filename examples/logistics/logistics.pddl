(define (domain logistics)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:types OBJ ; abstract data type
          POS ; abstract data type
          PACKAGE::OBJ
          TRUCK::OBJ
          AIRPLANE::OBJ
          LOCATION::POS
          AIRPORT::POS
          CITY)
  (:functions (position OBJ POS)
              (in PACKAGE OBJ)
              (pos_city POS CITY))
  (:objects mobile - POS none - OBJ)

  (:action LOAD_TRUCK
    :parameters ?pkg - PACKAGE ?truck - TRUCK
    :effect (:when (= (position ?pkg) (position ?truck))
              (:set (position ?pkg) mobile)
              (:set (in ?pkg) ?truck))
  )

  (:action LOAD_AIRPLANE
    :parameters ?pkg - PACKAGE ?airplane - AIRPLANE
    :effect (:when (= (position ?pkg) (position ?airplane))
              (:set (position ?pkg) mobile)
              (:set (in ?pkg) ?airplane))
  )

  (:action UNLOAD_TRUCK
    :parameters ?pkg - PACKAGE ?truck - TRUCK
    :effect (:when (= (in ?pkg) ?truck)
              (:set (in ?pkg) none)
              (:set (position ?pkg) (position ?truck)))
  )

  (:action UNLOAD_AIRPLANE
    :parameters ?pkg - PACKAGE ?airplane - AIRPLANE
    :effect (:when (= (in ?pkg) ?airplane)
              (:set (in ?pkg) none)
              (:set (position ?pkg) (position ?airplane)))
  )

  (:action DRIVE_TRUCK
    :parameters ?truck - TRUCK ?pos1 - POS ?pos2 - POS
    :precondition (:and (= (position ?truck) ?pos1)
			(= (pos_city ?pos1) (pos_city ?pos2)))
    :effect (:set (position ?truck) ?pos2)
  )

  (:action FLY_AIRPLANE
    :parameters ?airplane - AIRPLANE ?airport1 - AIRPORT ?airport2 - AIRPORT
    :precondition (= (position ?airplane) ?airport1)
    :effect (:set (position ?airplane) ?airport2)
  )
)

(define (problem p1)   
  (:domain logistics)
  (:objects package1 - PACKAGE
            pgh_truck bos_truck - TRUCK
            airplane1 - AIRPLANE
            bos_loc pgh_loc - LOCATION
            bos_airport pgh_airport - AIRPORT
            pgh bos - CITY
  )
  (:init (:set (position package1) :in { pgh_loc pgh_airport bos_loc bos_airport })
	 (:set (position pgh_truck) pgh_loc)
	 (:set (position bos_truck) bos_loc)
         (:set (position airplane1) bos_airport)
         (:set (in package1) none)
         (:set (pos_city bos_loc) bos)
         (:set (pos_city pgh_loc) pgh)
         (:set (pos_city bos_airport) bos)
         (:set (pos_city pgh_airport) pgh)
  )
  (:goal (= (position package1) bos_airport))
)

    MPI::Init ( argc, argv );
    id = MPI::COMM_WORLD.Get_rank ( );
    p = MPI::COMM_WORLD.Get_size ( );
    m = 100;
    n = 50;
    tag_done = m + 1;
    //  The master process allocates and initializes A and x.
    if ( id == 0 ) {
        a = new double[m*n];
        x = new double[n];
        b = new double[m];
    }
    //  Worker processes set aside room for one row of A, and for the
    //  vector x.
    else {
        a_row = new double[n];
        x = new double[n];
    }
    //  Process 0 broadcasts the vector X to the other processes.
    MPI::COMM_WORLD.Bcast ( x, n, MPI::DOUBLE, 0 );
    if ( id == 0 ) {
        //  Process 0 sends one row of A to all the other processes.
        num_rows = 0;

        for ( i = 1; i <= p-1; i++ ) {
            dest = i;
            tag = num_rows;
            k = num_rows * n;

            MPI::COMM_WORLD.Send ( a+k, n, MPI::DOUBLE, dest, tag );

            num_rows = num_rows + 1;
        }
        num_workers = p-1;

        for ( ; ; ) {
            MPI::COMM_WORLD.Recv ( &ans, 1, MPI::DOUBLE, MPI::ANY_SOURCE,
                    MPI::ANY_TAG, status );

            tag = status.Get_tag();
            b[tag] = ans;

            if ( num_rows < m ) {
                num_rows = num_rows + 1;
                dest = status.Get_source();
                tag = num_rows;
                k = num_rows * n;

                MPI::COMM_WORLD.Send ( a+k, n, MPI::DOUBLE, dest, tag );
            }
            else {
                num_workers = num_workers - 1;
                dummy = 0;
                dest = status.Get_source();
                tag = tag_done;

                MPI::COMM_WORLD.Send ( &dummy, 1, MPI::INT, dest, tag );
            }
        }

        delete [] a;
        delete [] x;
    }
    //  Each worker process repeatedly receives rows of A (with TAG indicating
    //  which row it is), computes dot products A(I,1:N) * X(1:N) and returns
    //  the result (and TAG), until receiving the "DONE" message.
    else {
        for ( ; ; ) {
            MPI::COMM_WORLD.Recv ( a_row, n, MPI::DOUBLE, 0, MPI::ANY_TAG,
                    status );

            tag = status.Get_tag();

            if ( tag == tag_done )
                break;

            ans = 0.0;
            for ( i = 0; i < n; i++ )
                ans = ans + a_row[i] * x[i];

            MPI::COMM_WORLD.Send ( &ans, 1, MPI::DOUBLE, 0, tag );

        }

        delete [] a_row;
        delete [] x;
    }
    //  Terminate MPI.
    MPI::Finalize ( );

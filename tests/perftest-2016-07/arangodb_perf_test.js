(function(){
    'use strict'
    var fs = require('fs')
    var db = require('internal').db

    function load_data(directory, col){
        print("get files in: " + directory  )
        var files = fs.list(directory)
        if (files.length == 0){
            print("no files to load");
            col = null;
            return col;
        }
        files.sort()
        print("OBI" + col)
        var buffer;
        var uid = 0;
        files.forEach( function(file){
            print("reading: " + file)
            buffer=fs.readFileSync(directory + fs.pathSeparator + file)
            var lines = buffer.toString().split('\n')

            lines.forEach(function(line){
                col.save({ "_key" : uid.toString()
                         , "data" : line
                         });
                uid += 1;
            })
        })
        return col
    }

    function get_collection(path, name, drop){
        print("enter create collection")
        if (drop){
            print("drop")
            db._drop(name)
        }
        var col = db[name];
        if (col) {
            print("returning existing collection")
            col.load()
            return col;
        }
        print("create new")
        col = db._create(name)
        col = load_data(path, col)
        return col
    }

    function run_test(col){
        print("starting test")
        var time = require('internal').time
        var start = time()

        // //very slow
        // var biggest_key = function(col){
        //     db._query(
        //     ` FOR doc IN @@col
        //         let key = to_number(doc._key)
        //         SORT key
        //         LIMIT 1
        //         RETURN key
        //     `, { "@col" : col.name() }
        //     )
        // }(col)


        print(col.properties());
        var count = col.count().toString();
        print("there are " + count + " elements in the collection");

        function next_element_all(current, count, prim){
            return (( current + prim ) % count)
        }

        var prim = Math.pow(2,31) - 1;
        print("prime: " + prim.toString());
        var key = 1;

        var test_count = count / 10;
        print(test_count)
        for(var i = 0; i < test_count/10; ++i){
            col.document(key.toString())
            key = next_element_all(key,count,prim);
        }

        return (time() - start)
    }

    function main(path, name){
        var col = get_collection(path, name, false)
        if(!col){
            print("no collection")
            return 1
        }
        var duration = run_test(col)

        print("total time taken: " + duration.toString())
        //print(col.figures())
        return 0
    }

    return main;
})()

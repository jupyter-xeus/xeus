var assert = require('assert');
var XeusModuleFactory = require('./emscripten_wasm_test.js')

function test_start_kernel(Module) {
    raw_xkernel = new Module.xkernel();
    var raw_xserver = raw_xkernel.get_server();
    raw_xkernel.start()
}

function test_buffer_sequence_js_to_cpp(XeusModule) {
    
    // create a binary buffer sequence on js side 
    // with various types
    buffers = [
        new Int8Array([-1,2,-3,4,-5]),
        new Uint8Array([10,20,30,40,50]),
        new Float32Array([1.5, 2.5, 3.5, 4.5]),
        new Int32Array([-10,10,20, 30, 40])
    ]

    // from js to c++
    buffer_sequence = new XeusModule.buffer_sequence() 
    buffer_sequence.from_js(buffers)

    // And for the fun of it we go back to js:
    // c++ to js without a copy!
    int8_restored_buffers = buffer_sequence.view()

    // recover convert to original type
    restored_buffers = [
        new Int8Array(   int8_restored_buffers[0].buffer, int8_restored_buffers[0].byteOffset, int8_restored_buffers[0].length),
        new Uint8Array(  int8_restored_buffers[1].buffer, int8_restored_buffers[1].byteOffset, int8_restored_buffers[1].length),
        new Float32Array(int8_restored_buffers[2].buffer, int8_restored_buffers[2].byteOffset, int8_restored_buffers[2].length / 4),
        new Int32Array(  int8_restored_buffers[3].buffer, int8_restored_buffers[3].byteOffset, int8_restored_buffers[3].length / 4)
    ]

    assert.deepEqual(buffers[0], restored_buffers[0])
    assert.deepEqual(buffers[1], restored_buffers[1])
    assert.deepEqual(buffers[2], restored_buffers[2])
    assert.deepEqual(buffers[3], restored_buffers[3])

}

function test_notify_listener(Module) {
    raw_xkernel = new Module.xkernel();
    var raw_xserver = raw_xkernel.get_server();
    raw_xkernel.start()

    header = {
        'msg_id'    : "550e8400-e29b-11d4-a716-446655440000",
        'session'   : "550e8400-e29b-11d4-a716-446655440000",
        'username'  : "name",
        'date'      : "2019-09-07T-15:50+00",
        'msg_type'  : "test_message",
        'version'   : '5.0'
    }

    message = {
        'header' : header,
        'parent_header': header,
        'metadata' : {
            "fo" : "bar"
        },
        'content' : {
            'fobar' : "fobar"
        },
        'channel' : 'shell',
        'buffers' : [
            new Int8Array([-1,2,-3,4,-5]),
            new Uint8Array([10,20,30,40,50]),
            new Float32Array([1.5, 2.5, 3.5, 4.5])
        ]   
    }
    console.log("IGNORE NEXT ERROR")
    raw_xserver.notify_listener(message)
}

async function test() {
    XeusModuleFactory().then((XeusModule) => {
        test_buffer_sequence_js_to_cpp(XeusModule)
        test_start_kernel(XeusModule)
        test_notify_listener(XeusModule)
    })
}

test();

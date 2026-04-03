// GradientWisp is completely synchronous, except for the |init| function cuz uh fuck you ig

var GradientWisp=(function(){
	var en=(function(a){return(a).encode.bind(a)})(new TextEncoder()), de=(function(a){return(a).decode.bind(a)})(new TextDecoder()), worker, conns = [{open:false}], defMaxPkt=0
	function idxExists(idx) {
		return((worker)&&((conns[idx])&&(conns[idx].open)))
	}
	function sendBuf(buf) {worker.postMessage(buf)}
	function sendBufFromIdx(buf, idx) {
		if (!(idxExists(idx))) return sendBuf(buf)
		if (conns[idx].sentPkts.length<conns[idx].maxPkt) sendBuf(buf)
		conns[idx].sentPkts.push(buf)
	}
	function waitForResp() {
		return new Promise(function(r) {
			worker.addEventListener("message", function({data:m}) {r(m)},{once:true})
		})
	}
	function n2uint32(n) {
		return[n&255,(n>>8)&255,(n>>16)&255,(n>>24)&255]
	}
	function l2n(l) {
		return(l).reduce(function([a,b],c){return[a|(c<<b),b+8]},[0,0])[0]
	}
	function handleCONTINUE(idx, newMaxPkt) {
		var prevMaxPkt = conns[idx].maxPkt
		conns[idx].maxPkt = newMaxPkt
		var leftOut = conns[idx].sentPkts.slice(prevMaxPkt,prevMaxPkt+conns[idx].maxPkt+1)
		var newLeftOut = conns[idx].sentPkts.slice(prevMaxPkt+conns[idx].maxPkt+1)
		conns[idx].sentPkts = []
		leftOut.forEach(function(p){sendBufFromIdx(p,idx)})
		conns[idx].sentPkts = conns[idx].sentPkts.concat(newLeftOut)
	}
	return {
		init(server) { // expects a ws url rather than a wisp url
			if (worker) return
			var wUrl = URL.createObjectURL(new Blob([`!function() {
				var ws
				function sendBuf(buf) {postMessage(Array.from(buf))}
				self.addEventListener("message", function({data:m}) {
					if (!((Object.prototype.toString.call(m)=="[object ArrayBuffer]")||(ws))) {
						var u
						try {u=new URL(m+"")} catch (_) {return}
						ws = new WebSocket(u.href)
						ws.binaryType = "arraybuffer"
						ws.addEventListener("message",function({data}){postMessage(Array.from(new Uint8Array(data)))}) // fuck it we ball
						ws.addEventListener("open",function(){postMessage(["open"])})
						ws.addEventListener("error",function(e){postMessage(["error","error from worker"])})
						ws.addEventListener("close",function(){postMessage(["close","closed?"])})
						return
					}
					if(ws)ws.send(new Uint8Array(m).buffer)
				})
			}()`],{type:"application/javascript"}))
			worker = new Worker(wUrl)
			URL.revokeObjectURL(wUrl)
			worker.postMessage(server)
			worker.addEventListener("message", function({data:m}) {
				var idx
				switch (m) {
					case "error":
					case "close":
						worker.terminate()
						worker=(function(){var a;return a})()
						conns=conns.map(function(a){return{...a,open:false}})
						return // bail out of function early
					default: break
				}
				if (!((Array.isArray(m))&&(typeof m[0]=="number"))) return
				if ((idx=l2n(m.slice(1,5)))>=conns.length) return // there is NO WAY l2n returns a negative number, right?
				if ((idx==0)&&(m[0]==3)) { // default max packets setting
					defMaxPkt = l2n(m.slice(5))
					conns.forEach(function(c,idx) {
						if (idx==0) return // nuh uh-uh!
						if (c.maxPkt>defMaxPkt) return // due to js' high acceptability of bullshit, even if c.maxPkt is undefined, it'll not error.
						handleCONTINUE(idx,defMaxPkt)
					})
				}
				if (!(conns[idx].open)) return
				if (m[0]==4) return conns[idx].open = false
				if (m[0]!=2) {
					switch (m[0]) {
						case 3:	// CONTINUE
							handleCONTINUE(idx,l2n(m.slice(5)))
							break
						case 4: // CLOSE
							conns[idx].open = false
					}
					return
				}
				conns[idx].msgQ.push(m.slice(5))
			})
			return new Promise(function(ok,err) {
				worker.addEventListener("message", function({data:m}) {
					(m[0]=="open"?ok:err)(m[1])
				},{once:true})
			})
		},
		connect(h, p) {
			if (!(worker)) return
			var idx = conns.length
			sendBuf([1, ...n2uint32(idx), 1, p&255, (p>>8)&255, ...en(h)])
			conns[idx] = {open:true,msgQ:[],maxPkt:defMaxPkt,sentPkts:[]}
			return idx
		},
		send2Idx(idx, buf) {
			if (!(idxExists(idx))) return
			// (hopefully) supports: strings, arraybuffers and any typed arrays (including Uint8Array)
			if (Object.prototype.toString.call(buf)=="[object String]") buf = en(buf)
			if (Object.prototype.toString.call(buf)=="[object ArrayBuffer]") buf = new Uint8Array(buf)
			if (Object.prototype.toString.call(buf)!="[object Uint8Array]") buf = new Uint8Array(buf.buffer) // for other typed arrays
			sendBufFromIdx([2, ...n2uint32(idx), ...Array.from(buf)], idx)
		},
		recv(idx) {
			if (!((worker)&&(conns[idx]))) return new ArrayBuffer() // idgaf if idx is closed. if there's smth to offer, go on.
			return new Uint8Array(((conns[idx].msgQ.shift())||([])).slice(5)).buffer
		},
		isIdxOpen(idx) {
			return Boolean(idxExists(idx)) // idxExists already checks for open state, so we're fine just throwing that in, heh.
		},
		close(idx) {
			if (!(idxExists(idx))) return
			conns[idx].open = false
			sendBufFromIdx([4, ...n2uint32(idx), 2], idx) // i'd say it *was* very voluntary.
		}
	}
})()

export default GradientWisp

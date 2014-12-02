TestUnit
====

```Ruby
class LoginTest < GeniusTestUnit
  def query
    pkt = LoginRequest.new
    pkt.id = "pjc0247"
    pkt.pw = "asdf1234"
    return pkt
  end
  
  def should
    resp = LoginResponse.new
    resp.result = true
    resp.nickname = "asdf"
    return resp
  end
end
```

pre/post_test로 선행/후행 작업을 지정할 수 있다.<br>
예) GameReadyTest는 방에 진입 한 후에 테스트 되어야 하며, 테스트가 종료된 이후에는 방에서 나가야 한다.
```Ruby
class GameReadyTest < GeniusTestUnit
  pre_test :EnterRoomTest
  post_test :LeaveRoom Test
end
```
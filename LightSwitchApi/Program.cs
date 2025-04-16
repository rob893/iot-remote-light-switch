var builder = WebApplication.CreateBuilder(args);

var app = builder.Build();

app.UseHttpsRedirection();

app.MapGet("/lightSwitchState", () =>
{
    return LoadStatus();
})
.WithName("GetLightSwitchState");

app.MapPost("/lightSwitchState/toggle", (ToggleRequest request) =>
{
    string correctPassword = app.Configuration["Password"] ?? throw new Exception("Password not set in config");

    if (request.Password != correctPassword)
    {
        return Results.Unauthorized();
    }

    var currStatus = LoadStatus();
    var newStatus = !currStatus;
    SaveStatus(newStatus);

    return Results.Ok(newStatus);
})
.WithName("ToggleLightSwitchState");

app.MapGet("/lightSwitchState/history", () =>
{
    return LoadHistory();
})
.WithName("GetLightSwitchHistory");

app.MapPost("/lightSwitchState/deviceCallback", (DeviceStateCallback callback) =>
{
    var history = LoadHistory();
    history.Add(new StateChangeEntry
    {
        Timestamp = DateTime.UtcNow,
        Success = callback.Success,
        ErrorMessage = callback.ErrorMessage,
        AttemptedState = callback.AttemptedState
    });
    SaveHistory(history);
    return Results.Ok();
})
.WithName("DeviceStateCallback");

app.Run();

static void SaveStatus(bool flag)
{
    File.WriteAllText("flag.txt", flag.ToString());
}

static bool LoadStatus()
{
    var loadedFlag = true;

    if (File.Exists("flag.txt"))
    {
        string content = File.ReadAllText("flag.txt");
        if (bool.TryParse(content, out bool result))
        {
            loadedFlag = result;
        }
    }

    return loadedFlag;
}

static void SaveHistory(List<StateChangeEntry> history)
{
    string json = System.Text.Json.JsonSerializer.Serialize(history, new System.Text.Json.JsonSerializerOptions
    {
        WriteIndented = true
    });
    File.WriteAllText("history.json", json);
}

static List<StateChangeEntry> LoadHistory()
{
    if (!File.Exists("history.json"))
    {
        return new List<StateChangeEntry>();
    }

    string json = File.ReadAllText("history.json");
    try
    {
        var history = System.Text.Json.JsonSerializer.Deserialize<List<StateChangeEntry>>(json);
        return history ?? new List<StateChangeEntry>();
    }
    catch
    {
        return new List<StateChangeEntry>();
    }
}

record ToggleRequest(string Password);
record DeviceStateCallback(bool Success, string? ErrorMessage, bool AttemptedState);
record StateChangeEntry
{
    public DateTime Timestamp { get; init; }
    public bool Success { get; init; }
    public string? ErrorMessage { get; init; }
    public bool AttemptedState { get; init; }
}